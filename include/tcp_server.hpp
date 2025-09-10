#pragma once

#include <cstdint>

namespace GunBound::Broker {

/*!
 * @brief Inicia o servidor TCP em uma thread dedicada.
 *
 * Cria um listener em 127.0.0.1:port e processa conexões sequencialmente,
 * enviando o payload recebido para OutputDebugStringA. Chamadas repetidas
 * não têm efeito após a primeira inicialização.
 *
 * @param port Porta TCP (padrão: 8625).
 */
void start_server(std::uint16_t port = 8625) noexcept;

/*!
 * @brief Solicita desligamento e aguarda a thread finalizar brevemente.
 */
void stop_server() noexcept;

} /* namespace GunBound::Broker */
