#include "protocol/packet.hpp"
#include "helpers/endian.hpp"

#include <algorithm>

namespace GunBound::Broker::Packets {

Packet make_packet(std::uint16_t len, std::uint16_t seq, std::uint16_t id, std::vector<std::uint8_t> data) {
    Packet p{};
    p.seq = seq;
    p.id = id;
    p.payload = std::move(data);
    if (len == 0) {
        p.len = static_cast<std::uint16_t>(6u + p.payload.size());
    } else {
        p.len = len;
    }
    return p;
}

std::optional<Packet> parse(const std::uint8_t *data, std::size_t size) noexcept {
    if (size < 6)
        return std::nullopt;
    const std::uint16_t len = Utils::read_uint16_le(data);
    if (len < 6)
        return std::nullopt;
    if (size < len)
        return std::nullopt;

    Packet p{};
    p.len = len;
    p.seq = Utils::read_uint16_le(data + 2);
    p.id = Utils::read_uint16_le(data + 4);
    const std::size_t pay = static_cast<std::size_t>(len) - 6;
    p.payload.assign(data + 6, data + 6 + pay);
    return p;
}

std::vector<std::uint8_t> serialize(const Packet &p) {
    std::vector<std::uint8_t> out;
    out.resize(6 + p.payload.size());
    Utils::write_uint16_le_at(out.data() + 0, p.len);
    Utils::write_uint16_le_at(out.data() + 2, p.seq);
    Utils::write_uint16_le_at(out.data() + 4, p.id);
    if (!p.payload.empty()) {
        std::copy(p.payload.begin(), p.payload.end(), out.begin() + 6);
    }
    return out;
}

} // namespace GunBound::Broker::Packets
