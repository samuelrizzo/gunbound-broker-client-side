#pragma once

#include <cstdint>
#include <string_view>

namespace GunBound::Broker::Debug {

/*!
 * @brief Envia texto para OutputDebugStringA, garantindo terminação por '\n'.
 * @param sv Texto a ser enviado.
 */
void debug_print(std::string_view sv) noexcept;

/*!
 * @brief Registra um erro do Winsock/WSA no OutputDebugStringA.
 * @param where Contexto ou nome da chamada que falhou.
 * @param err Código de erro retornado (WSAGetLastError).
 */
void log_wsa_error(const char *where, int err) noexcept;

/*!
 * @brief Registra o tamanho e o payload em hexadecimal de um pacote recebido.
 * @param data Ponteiro para o buffer recebido.
 * @param size Quantidade de bytes no buffer.
 */
void log_packet(const void *data, std::size_t size) noexcept;

/*!
 * @brief Loga linha rotulada com cabeçalho e payload em hex.
 * Formato: [LABEL] LEN: <dec> - SEQ: 0x<hex4> - PROTOCOL: 0x<hex4> - DATA: <hex bytes>
 */
void log_packet_labeled(const char *label, std::uint16_t len, std::uint16_t seq, std::uint16_t id,
                        const std::uint8_t *payload, std::size_t payloadLen) noexcept;

} /* namespace GunBound::Broker::Debug */
