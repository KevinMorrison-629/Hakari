#include "core/net/NetworkManager.h"

#include <iostream>

namespace Core::Net
{
    // The static callback function is the global entry point for connection status changes.
    void NetworkManager::OnGlobalConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo)
    {
        // The user data we set during connection/listen is a pointer to our NetworkManager instance.
        NetworkManager *manager = (NetworkManager *)pInfo->m_info.m_nUserData;
        if (manager)
        {
            manager->HandleConnectionStatusChanged(pInfo);
        }
    }

    NetworkManager::NetworkManager() : m_pInterface(nullptr)
    {
        // Initialize the GameNetworkingSockets library.
        SteamDatagramErrMsg errMsg;
        if (!GameNetworkingSockets_Init(nullptr, errMsg))
        {
            std::cerr << "FATAL: GameNetworkingSockets_Init failed. " << errMsg << std::endl;
        }
        else
        {
            m_pInterface = SteamNetworkingSockets();
        }
    }

    NetworkManager::~NetworkManager()
    {
        // Shutdown the library.
        GameNetworkingSockets_Kill();
    }

    void NetworkManager::Poll()
    {
        if (!m_pInterface)
            return;
        // This is the heart of the manager. It triggers all callbacks for connection
        // status changes, which are then handled by the derived classes.
        m_pInterface->RunCallbacks();
    }
}