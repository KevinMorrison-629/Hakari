#include "core/net/ClientManager.h"

#include <iostream>

namespace Core::Net
{
    bool ClientManager::Connect(const std::string &strServerAddress)
    {
        if (!m_pInterface)
            return false;

        SteamNetworkingIPAddr serverAddr;
        const char *srvAddress = strServerAddress.c_str();
        if (!serverAddr.ParseString(srvAddress))
        {
            std::cerr << "Invalid server address: " << strServerAddress << std::endl;
            return false;
        }

        // Set up configuration options for the connection.
        SteamNetworkingConfigValue_t opts[2];
        opts[0].SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void *)NetworkManager::OnGlobalConnectionStatusChanged);
        opts[1].SetInt64(k_ESteamNetworkingConfig_ConnectionUserData, (int64)this);

        m_hConnection = m_pInterface->ConnectByIPAddress(serverAddr, 2, opts);
        if (m_hConnection == k_HSteamNetConnection_Invalid)
        {
            std::cerr << "Failed to create connection." << std::endl;
            return false;
        }

        return true;
    }

    void ClientManager::Disconnect()
    {
        if (!m_pInterface || m_hConnection == k_HSteamNetConnection_Invalid)
            return;

        m_pInterface->CloseConnection(m_hConnection, 0, "Client disconnecting", true);
        m_hConnection = k_HSteamNetConnection_Invalid;
    }

    void ClientManager::SendMessageToServer(const std::string &strMessage)
    {
        if (!IsConnected())
            return;
        m_pInterface->SendMessageToConnection(m_hConnection, strMessage.c_str(), strMessage.length(), k_nSteamNetworkingSend_Reliable, nullptr);
    }

    bool ClientManager::IsConnected() const
    {
        // A connection is considered active if its handle is not invalid.
        return m_hConnection != k_HSteamNetConnection_Invalid;
    }

    void ClientManager::HandleConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo)
    {
        // The client only cares about events for its single connection.
        if (pInfo->m_hConn != m_hConnection)
            return;

        switch (pInfo->m_info.m_eState)
        {
        case k_ESteamNetworkingConnectionState_Connected:
            std::cout << "Client: Successfully connected to server." << std::endl;
            break;

        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
        {
            std::cout << "Client: Disconnected from server. Reason: " << pInfo->m_info.m_szEndDebug << std::endl;
            m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
            m_hConnection = k_HSteamNetConnection_Invalid; // Mark as disconnected.
            break;
        }

        default:
            // Other states like 'Connecting' are transitional and don't need handling here.
            break;
        }
    }

    void ClientManager::ReceiveMessages()
    {
        if (!IsConnected())
            return;

        ISteamNetworkingMessage *pIncomingMsgs[16];
        int numMsgs = m_pInterface->ReceiveMessagesOnConnection(m_hConnection, pIncomingMsgs, 16);
        if (numMsgs > 0)
        {
            for (int i = 0; i < numMsgs; ++i)
            {
                if (pIncomingMsgs[i] && pIncomingMsgs[i]->m_cbSize > 0)
                {
                    std::string msg((const char *)pIncomingMsgs[i]->m_pData, pIncomingMsgs[i]->m_cbSize);

                    // If the application has set a callback, use it.
                    if (OnMessageReceived)
                    {
                        OnMessageReceived(msg);
                    }

                    pIncomingMsgs[i]->Release();
                }
            }
        }
    }
}