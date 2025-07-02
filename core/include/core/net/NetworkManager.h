#pragma once

#include <steam/steamnetworkingsockets.h>
#include <functional>

namespace Core::Net
{
    /// @brief Base class for network operations using SteamNetworkingSockets.
    /// This class provides common functionality for client and server network management,
    /// such as polling for network events and handling connection status changes.
    class NetworkManager
    {
    public:
        /// @brief Constructor for NetworkManager.
        /// Initializes the SteamNetworkingSockets library and acquires the interface.
        NetworkManager();

        /// @brief Virtual destructor for NetworkManager.
        /// Ensures proper cleanup of network resources, including shutting down the SteamNetworkingSockets library.
        virtual ~NetworkManager();

        /// @brief Polls for network events.
        /// This method should be called regularly to process incoming messages and connection status changes.
        void Poll();

    protected:
        /// @brief Pure virtual function to handle connection status changes.
        /// Derived classes must implement this method to process specific connection events.
        /// @param pInfo Pointer to the SteamNetConnectionStatusChangedCallback_t structure containing event details.
        virtual void HandleConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo) = 0;

        /// @brief Static callback function for global connection status changes.
        /// This function is registered with SteamNetworkingSockets and dispatches events
        /// to the appropriate NetworkManager instance.
        /// @param pInfo Pointer to the SteamNetConnectionStatusChangedCallback_t structure.
        static void OnGlobalConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo);

        /// @brief Pointer to the ISteamNetworkingSockets interface.
        ISteamNetworkingSockets *m_pInterface;

    private:
    };
}