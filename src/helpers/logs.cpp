#include "helpers/logs.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <windows.h>

namespace GunBound::Broker::Debug {

/*!
 * @file
 * @brief Implementação das rotinas de log para OutputDebugStringA.
 */

void debug_print(std::string_view sv) noexcept {
    std::string line{sv};
    if (line.empty() || line.back() != '\n')
        line.push_back('\n');
    OutputDebugStringA(line.c_str());
}

void log_wsa_error(const char *where, int err) noexcept {
    char buf[128]{};
    _snprintf_s(buf, _TRUNCATE, "%s failed. WSA error=%d\n", where, err);
    OutputDebugStringA(buf);
}

void log_packet(const void *data, std::size_t size) noexcept {
    const auto *bytes = static_cast<const unsigned char *>(data);
    std::string out;
    out.reserve(16 + 3 * (size > 0 ? size : 1));
    out += "recv ";
    out += std::to_string(static_cast<unsigned long long>(size));
    out += " bytes: ";

    char tmp[4];
    for (std::size_t i = 0; i < size; ++i) {
        _snprintf_s(tmp, _TRUNCATE, "%02X", bytes[i]);
        out += tmp;
        if (i + 1 < size)
            out.push_back(' ');
    }

    debug_print(out);
}

void log_packet_labeled(const char *label, std::uint16_t len, std::uint16_t seq, std::uint16_t id,
                        const std::uint8_t *payload, std::size_t payloadLen) noexcept {
    std::string out;
    out.reserve(64 + 3 * (payloadLen > 0 ? payloadLen : 1));
    out += '[';
    out += label ? label : "";
    out += "] LEN: ";
    out += std::to_string(static_cast<unsigned long long>(len));
    out += " - SEQ: 0x";
    char tmp[8];
    _snprintf_s(tmp, _TRUNCATE, "%04X", seq);
    out += tmp;
    out += " - PROTOCOL: 0x";
    _snprintf_s(tmp, _TRUNCATE, "%04X", id);
    out += tmp;
    out += " - DATA: ";
    for (std::size_t i = 0; i < payloadLen; ++i) {
        _snprintf_s(tmp, _TRUNCATE, "%02X", payload[i]);
        out += tmp;
        if (i + 1 < payloadLen)
            out.push_back(' ');
    }
    debug_print(out);
}

} /* namespace GunBound::Broker::Debug */
