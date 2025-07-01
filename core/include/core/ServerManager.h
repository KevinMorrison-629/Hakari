#pragma once

#include "core/NetworkManager.h"

#include <vector>
#include <string>

class ServerManager : public NetworkManager {
public:
    bool Start(uint16 nPort);
    void Stop();
    void BroadcastMessage(const std::string& strMessage);
    void ReceiveMessages();

protected:
    virtual void HandleConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo) override;

private:
    
    HSteamListenSocket m_hListenSocket;
    std::vector<HSteamNetConnection> m_vecClients;
};