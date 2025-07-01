#include "core/ServerManager.h"

#include <iostream>
#include <algorithm> // For std::remove

bool ServerManager::Start(uint16 nPort) {
    if (!m_pInterface) return false;

    SteamNetworkingIPAddr serverAddr;
    serverAddr.Clear();
    serverAddr.m_port = nPort;

    // Set up the configuration options for the listen socket.
    SteamNetworkingConfigValue_t opts[2];
    
    // First option: Set the callback function.
    // The static function in NetworkManager will dispatch to our instance method.
    opts[0].SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)NetworkManager::OnGlobalConnectionStatusChanged);

    // Second option: Set the user data to be a pointer to this object.
    // This is crucial for the static callback to find the correct instance.
    opts[1].SetInt64(k_ESteamNetworkingConfig_ConnectionUserData, (int64)this);

    m_hListenSocket = m_pInterface->CreateListenSocketIP(serverAddr, 2, opts);
    if (m_hListenSocket == k_HSteamListenSocket_Invalid) {
        std::cerr << "Failed to create listen socket on port " << nPort << std::endl;
        return false;
    }
    
    return true;
}

void ServerManager::Stop() {
    if (!m_pInterface) return;
    
    // Close all active client connections.
    for (HSteamNetConnection conn : m_vecClients) {
        m_pInterface->CloseConnection(conn, 0, "Server shutting down", true);
    }
    m_vecClients.clear();

    // Close the listen socket.
    if (m_hListenSocket != k_HSteamListenSocket_Invalid) {
        m_pInterface->CloseListenSocket(m_hListenSocket);
        m_hListenSocket = k_HSteamListenSocket_Invalid;
    }
}

void ServerManager::BroadcastMessage(const std::string& strMessage) {
    if (!m_pInterface) return;
    
    for (HSteamNetConnection hConn : m_vecClients) {
        m_pInterface->SendMessageToConnection(hConn, strMessage.c_str(), strMessage.length(), k_nSteamNetworkingSend_Reliable, nullptr);
    }
}

void ServerManager::HandleConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo) {
    switch (pInfo->m_info.m_eState) {
        case k_ESteamNetworkingConnectionState_Connecting: {
            std::cout << "Server: Connection request from " << pInfo->m_info.m_szConnectionDescription << std::endl;
            if (m_pInterface->AcceptConnection(pInfo->m_hConn) != k_EResultOK) {
                m_pInterface->CloseConnection(pInfo->m_hConn, 0, "Failed to accept", false);
                std::cout << "Server: Failed to accept connection." << std::endl;
            }
            break;
        }

        case k_ESteamNetworkingConnectionState_Connected: {
            std::cout << "Server: Client connected. ID: " << pInfo->m_hConn << std::endl;
            m_vecClients.push_back(pInfo->m_hConn);
            break;
        }

        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
            std::cout << "Server: Client disconnected. ID: " << pInfo->m_hConn << ". Reason: " << pInfo->m_info.m_szEndDebug << std::endl;
            m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);

            // Remove the client from our active list.
            auto it = std::remove(m_vecClients.begin(), m_vecClients.end(), pInfo->m_hConn);
            if (it != m_vecClients.end()) {
                m_vecClients.erase(it);
            }
            break;
        }

        default:
            break;
    }
}

void ServerManager::ReceiveMessages() {
    if (!m_pInterface) return;
    
    // This would be called inside your main loop after Poll() if you want to process messages.
    // For simplicity, this is not currently called in the provided server_main.cpp.
    for (HSteamNetConnection hConn : m_vecClients) {
        ISteamNetworkingMessage* pIncomingMsgs[16];
        int numMsgs = m_pInterface->ReceiveMessagesOnConnection(hConn, pIncomingMsgs, 16);
        if (numMsgs > 0) {
            for (int i = 0; i < numMsgs; ++i) {
                std::string msg((const char*)pIncomingMsgs[i]->m_pData, pIncomingMsgs[i]->m_cbSize);
                std::cout << "Received from client " << hConn << ": " << msg << std::endl;
                // Process the message...
                pIncomingMsgs[i]->Release();
            }
        }
    }
}