#pragma once

#include "core/net/NetworkManager.h"

#include <vector>
#include <string>

namespace Core::Net
{
    class ServerManager : public NetworkManager
    {
    public:
        bool Start(uint16 nPort);
        void Stop();
        void BroadcastMessage(const std::string &strMessage);
        void ReceiveMessages();

    protected:
        virtual void HandleConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo) override;

    private:
        HSteamListenSocket m_hListenSocket;
        std::vector<HSteamNetConnection> m_vecClients;
    };
}