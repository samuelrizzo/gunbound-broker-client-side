#include "protocol/handler.hpp"
#include "helpers/endian.hpp"
#include "protocol/ids.hpp"
#include "protocol/server_list.hpp"
#include <vector>

namespace GunBound::Broker::Protocol {

/*!
 * @file
 * @brief Protocol packet handler
 */

/**
 * @brief Builds the server list payload
 * @param list Vector of server game options
 * @return Serialized payload as byte vector
 */
std::vector<std::uint8_t> build_sorted_server_list_payload(const std::vector<ServerGameOption> &list) {
    std::vector<std::uint8_t> out;
    out.reserve(64 * list.size() + 4);

    Utils::write_uint16_le(out, 0x0000);
    out.push_back(0x01);
    out.push_back(static_cast<std::uint8_t>(list.size()));

    for (const auto &s : list) {
        Utils::write_uint16_le(out, s.id);
        out.push_back(s.button_image_id);

        out.push_back(static_cast<std::uint8_t>(s.title.size()));
        out.insert(out.end(), s.title.begin(), s.title.end());

        out.push_back(static_cast<std::uint8_t>(s.description.size()));
        out.insert(out.end(), s.description.begin(), s.description.end());

        out.insert(out.end(), s.ip.begin(), s.ip.end());

        Utils::write_uint16_be(out, s.port);
        Utils::write_uint16_le(out, 0x0000);
        Utils::write_uint16_le(out, 0x0447);
        Utils::write_uint16_le(out, 0x082F);

        out.push_back(static_cast<std::uint8_t>(s.min_rank));
        out.push_back(static_cast<std::uint8_t>(s.max_rank));
        out.push_back(s.avatar_enabled);
        out.push_back(s.enabled);
    }

    return out;
}

std::optional<Packets::Packet> handle_packet(const Packets::Packet &in) {
    switch (in.id) {
    case Ids::SVC_SORTED_SERVER_LIST: {
        ServerGameOption opt{};
        opt.id = 0x0000;
        opt.button_image_id = 0x00;
        opt.header_flag = 0x01;
        opt.title = "GunBound Server 01";
        opt.description = "Open Area";
        opt.ip = {0xAC, 0x41, 0xE3, 0x1C};
        opt.port = 0x21B4;
        opt.reserved = 0x0000;
        opt.current_players = 0x0006;
        opt.recommended_max = 0xE809;
        opt.min_rank = static_cast<std::int8_t>(0xFC);
        opt.max_rank = static_cast<std::int8_t>(0x13);
        opt.avatar_enabled = 0x00;
        opt.enabled = 0x01;

        std::vector<std::uint8_t> data = build_sorted_server_list_payload({opt});
        // comentado por rizzo em 10/09/2025
        // len + seq estão hardcoded
        auto pkt = Packets::make_packet(0x003A, 0x1355, Ids::SVC_SORTED_SERVER_LIST_RESPONSE, std::move(data));

        return pkt;
    }
    default:
        return Packets::make_packet(in.len, in.seq, in.id, in.payload);
    }
}

} /* namespace GunBound::Broker::Protocol */
