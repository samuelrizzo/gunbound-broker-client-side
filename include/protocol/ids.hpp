#pragma once

#include <cstdint>

namespace GunBound::Broker::Protocol::Ids {

/*!
 * @brief Identificadores dos protocolos da server list
 */
inline constexpr std::uint16_t SVC_SORTED_SERVER_LIST = 0x1100;
inline constexpr std::uint16_t SVC_SORTED_SERVER_LIST_RESPONSE = 0x1102;

} /* namespace GunBound::Broker::Protocol::Ids */