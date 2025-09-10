#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace GunBound::Broker::Packets {

/**
 * @brief Pacote básico do protocolo: len, seq, id e payload.
 *
 * Layout em wire (little-endian):
 *   len(2) | seq(2) | id(2) | payload(len-6)
 */
struct Packet {
    std::uint16_t len{};
    std::uint16_t seq{};
    std::uint16_t id{};
    std::vector<std::uint8_t> payload{};
};

/**
 * @brief Constrói um pacote a partir dos campos.
 *
 * Se len==0, o valor será calculado como 6 + payload.size().
 */
Packet make_packet(std::uint16_t len, std::uint16_t seq, std::uint16_t id, std::vector<std::uint8_t> data);

/**
 * @brief Tenta parsear um pacote completo de um buffer.
 *
 * Retorna std::nullopt se o buffer não contiver bytes suficientes para um pacote completo
 * ou se o tamanho for inválido (<6).
 */
std::optional<Packet> parse(const std::uint8_t *data, std::size_t size) noexcept;

/**
 * @brief Serializa o pacote para bytes.
 */
std::vector<std::uint8_t> serialize(const Packet &p);

} /* namespace GunBound::Broker::Packets */
