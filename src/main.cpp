#include <windows.h>

#include "tcp_server.hpp"

/**
 * @brief Entry point da DLL que inicia e encerra o servidor TCP.
 */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        // comentado por rizzo em 10/09/2025
        // TODO porta hardcoded, implementar fallback caso usuário já tenha um servidor rodando na porta 8625
        // sugiro implementar valor aleatório e fazer dar 'match' na dll do client
        GunBound::Broker::start_server(8625);
        break;
    case DLL_PROCESS_DETACH:
        GunBound::Broker::stop_server();
        break;
    default:
        break;
    }
    return TRUE;
}
