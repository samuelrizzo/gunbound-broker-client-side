#pragma once

#include <array>
#include <cstdint>
#include <string>

namespace GunBound::Broker::Protocol {

/**
 * @brief Game server option configuration
 */
struct ServerGameOption {
    std::uint16_t id{};
    std::uint8_t button_image_id{};
    std::uint8_t header_flag{};

    std::string title;
    std::string description;

    std::array<std::uint8_t, 4> ip{};
    std::uint16_t port{};
    std::uint16_t reserved{};
    std::uint16_t current_players{};
    std::uint16_t recommended_max{};
    std::int8_t min_rank{};
    std::int8_t max_rank{};
    std::uint8_t avatar_enabled{};
    std::uint8_t enabled{};
};

} /* namespace GunBound::Broker::Protocol */