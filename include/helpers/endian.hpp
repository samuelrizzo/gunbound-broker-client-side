#pragma once

#include <cstdint>
#include <vector>

namespace GunBound::Broker::Utils {

/*!
 * @brief Escreve um valor uint16 em little-endian no buffer.
 * @param buffer Buffer de destino.
 * @param value Valor a ser escrito.
 */
void write_uint16_le(std::vector<std::uint8_t>& buffer, std::uint16_t value) noexcept;

/*!
 * @brief Escreve um valor uint16 em big-endian no buffer.
 * @param buffer Buffer de destino.
 * @param value Valor a ser escrito.
 */
void write_uint16_be(std::vector<std::uint8_t>& buffer, std::uint16_t value) noexcept;

/*!
 * @brief Lê um valor uint16 em little-endian de um buffer.
 * @param data Ponteiro para os dados.
 * @return Valor uint16 lido.
 */
std::uint16_t read_uint16_le(const std::uint8_t* data) noexcept;

/*!
 * @brief Escreve um valor uint16 em little-endian no buffer em posição específica.
 * @param data Ponteiro para o buffer de destino.
 * @param value Valor a ser escrito.
 */
void write_uint16_le_at(std::uint8_t* data, std::uint16_t value) noexcept;

} // namespace GunBound::Broker::Utils