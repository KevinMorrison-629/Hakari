#pragma once

#include <steam/steamnetworkingsockets.h>
#include <functional>

namespace Core::Net
{
    class NetworkManager
    {
    public:
        NetworkManager();
        virtual ~NetworkManager();

        void Poll();

    protected:
        virtual void HandleConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo) = 0;
        static void OnGlobalConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo);

        ISteamNetworkingSockets *m_pInterface;

    private:
    };
}