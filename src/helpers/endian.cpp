#include "helpers/endian.hpp"

namespace GunBound::Broker::Utils {

void write_uint16_le(std::vector<std::uint8_t>& buffer, std::uint16_t value) noexcept {
    buffer.push_back(static_cast<std::uint8_t>(value & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
}

void write_uint16_be(std::vector<std::uint8_t>& buffer, std::uint16_t value) noexcept {
    buffer.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>(value & 0xFF));
}

std::uint16_t read_uint16_le(const std::uint8_t* data) noexcept {
    return static_cast<std::uint16_t>(data[0] | (std::uint16_t(data[1]) << 8));
}

void write_uint16_le_at(std::uint8_t* data, std::uint16_t value) noexcept {
    data[0] = static_cast<std::uint8_t>(value & 0xFF);
    data[1] = static_cast<std::uint8_t>((value >> 8) & 0xFF);
}

} // namespace GunBound::Broker::Utils