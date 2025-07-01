#pragma once

#include "core/NetworkManager.h"

#include <steam/steamnetworkingsockets.h>
#include <string>

class ClientManager : public NetworkManager {
public:
    bool Connect(const std::string& strServerAddress);
    void Disconnect();
    void SendMessageToServer(const std::string& strMessage);
    void ReceiveMessages();
    bool IsConnected() const;

    std::function<void(const std::string&)> OnMessageReceived;

protected:
    virtual void HandleConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo) override;

private:
    HSteamNetConnection m_hConnection;
};