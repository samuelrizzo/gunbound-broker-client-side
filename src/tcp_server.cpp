#include "tcp_server.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "helpers/logs.hpp"
#include "protocol/handler.hpp"
#include "protocol/packet.hpp"

#include <atomic>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>

namespace GunBound::Broker {

namespace {

/*!
 * @file
 * @brief Implementação do servidor TCP baseado em Winsock que publica
 *        o payload recebido em OutputDebugStringA.
 */

std::atomic<bool> g_running{false};
std::atomic<bool> g_stop{false};
std::thread g_thread;

/*!
 * @brief Cria e coloca um socket em listen em 127.0.0.1:port.
 */
SOCKET make_listen_socket(std::uint16_t port) noexcept {
    SOCKET s = INVALID_SOCKET;

    s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        Debug::log_wsa_error("socket", WSAGetLastError());
        return INVALID_SOCKET;
    }

    BOOL opt = TRUE;
    ::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&opt), sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    // comentado por rizzo em 10/09/2025
    // nunca mudar o INADDR_LOOPBACK para outro valor, ele é o 127.0.0.1
    // é o valor que precisa estar na dll do client
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (::bind(s, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        Debug::log_wsa_error("bind", WSAGetLastError());
        ::closesocket(s);
        return INVALID_SOCKET;
    }

    if (::listen(s, SOMAXCONN) == SOCKET_ERROR) {
        Debug::log_wsa_error("listen", WSAGetLastError());
        ::closesocket(s);
        return INVALID_SOCKET;
    }

    return s;
}

/*!
 * @brief Recebe dados de um cliente e publica no debug output.
 */
void handle_client(SOCKET client) noexcept {
    std::vector<std::uint8_t> buf;
    buf.reserve(4096);

    std::uint8_t tmp[2048];
    for (;;) {
        int recvd = ::recv(client, reinterpret_cast<char *>(tmp), static_cast<int>(sizeof(tmp)), 0);
        if (recvd > 0) {
            buf.insert(buf.end(), tmp, tmp + recvd);

            for (;;) {
                if (buf.size() < 2)
                    break;
                std::uint16_t len = static_cast<std::uint16_t>(buf[0] | (std::uint16_t(buf[1]) << 8));
                if (len < 6) {
                    Debug::debug_print("invalid packet length < 6");
                    return;
                }
                if (buf.size() < len)
                    break;

                auto maybe = Packets::parse(buf.data(), len);
                if (!maybe) {
                    Debug::debug_print("parse failed");
                    return;
                }

                const auto pkt = *maybe;
                Debug::log_packet_labeled("RECV", pkt.len, pkt.seq, pkt.id, pkt.payload.data(), pkt.payload.size());

                auto reply = Packets::handle_packet(pkt);
                if (reply) {
                    auto bytes = Packets::serialize(*reply);
                    Debug::log_packet_labeled("SEND", reply->len, reply->seq, reply->id, reply->payload.data(),
                                              reply->payload.size());
                    std::size_t sent = 0;
                    while (sent < bytes.size()) {
                        int s = ::send(client, reinterpret_cast<const char *>(bytes.data() + sent),
                                       static_cast<int>(bytes.size() - sent), 0);
                        if (s == SOCKET_ERROR) {
                            break;
                        }
                        sent += static_cast<std::size_t>(s);
                    }
                }

                buf.erase(buf.begin(), buf.begin() + len);
            }
        } else if (recvd == 0) {
            break;
        } else {
            break;
        }
    }
    ::closesocket(client);
}

/*!
 * @brief Loop principal do servidor: inicia Winsock, aceita e atende conexões.
 */
void server_loop(std::uint16_t port) noexcept {
    WSADATA wsa{};
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        Debug::log_wsa_error("WSAStartup", WSAGetLastError());
        return;
    }

    SOCKET listener = make_listen_socket(port);
    if (listener == INVALID_SOCKET) {
        WSACleanup();
        return;
    }

    {
        std::ostringstream oss;
        oss << "GunBound Broker Client listening on 127.0.0.1:" << port;
        Debug::debug_print(oss.str());
    }

    while (!g_stop.load(std::memory_order_relaxed)) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(listener, &readfds);
        timeval tv{};
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ready = ::select(0, &readfds, nullptr, nullptr, &tv);
        if (ready == SOCKET_ERROR) {
            Debug::log_wsa_error("select", WSAGetLastError());
            break;
        }
        if (ready == 0) {
            continue;
        }

        if (FD_ISSET(listener, &readfds)) {
            sockaddr_in peer{};
            int len = sizeof(peer);
            SOCKET client = ::accept(listener, reinterpret_cast<sockaddr *>(&peer), &len);
            if (client == INVALID_SOCKET) {
                int err = WSAGetLastError();
                if (err == WSAEINTR)
                    continue;
                Debug::log_wsa_error("accept", err);
                if (g_stop.load(std::memory_order_relaxed))
                    break;
                continue;
            }

            {
                std::ostringstream oss;
                oss << "Client connected from " << inet_ntoa(peer.sin_addr) << ":" << ntohs(peer.sin_port);
                Debug::debug_print(oss.str());
            }

            handle_client(client);

            {
                std::ostringstream oss;
                oss << "Client disconnected from " << inet_ntoa(peer.sin_addr) << ":" << ntohs(peer.sin_port);
                Debug::debug_print(oss.str());
            }
        }
    }

    ::closesocket(listener);
    WSACleanup();
}

} /* namespace */

void start_server(std::uint16_t port) noexcept {
    bool expected = false;
    if (!g_running.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
        return;
    }
    g_stop.store(false, std::memory_order_relaxed);
    try {
        g_thread = std::thread([port] { server_loop(port); });
    } catch (...) {
        g_running.store(false, std::memory_order_release);
    }
}

void stop_server() noexcept {
    if (!g_running.load(std::memory_order_acquire))
        return;
    g_stop.store(true, std::memory_order_release);
    if (g_thread.joinable()) {
        for (int i = 0; i < 4; ++i) {
            if (g_stop.load(std::memory_order_relaxed)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
        try {
            g_thread.join();
        } catch (...) {
        }
    }
    g_running.store(false, std::memory_order_release);
}

} /* namespace GunBound::Broker */
