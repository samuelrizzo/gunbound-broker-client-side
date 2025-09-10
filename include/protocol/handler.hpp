#pragma once

#include "protocol/packet.hpp"
#include <optional>

namespace GunBound::Broker::Protocol {

/**
 * @brief Processa um pacote recebido e gera uma resposta.
 */
std::optional<Packets::Packet> handle_packet(const Packets::Packet &in);

} /* namespace GunBound::Broker::Protocol */

namespace GunBound::Broker::Packets {
    using GunBound::Broker::Protocol::handle_packet;
}
