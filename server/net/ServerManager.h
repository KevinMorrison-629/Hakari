#pragma once

#include "common/net/ConnectionManager.h"
#include "server/core/TaskManager.h"

#include <string>
#include <vector>

namespace Core::Net
{
    /// @brief Manages the server-side network operations, including listening for client connections.
    /// This class handles starting and stopping the server, broadcasting messages to clients,
    /// and managing connected clients. It inherits from ConnectionManager.
    class ServerManager : public ConnectionManager
    {
    public:
        /// @brief Starts the server and begins listening for incoming connections on the specified port.
        /// @param nPort The port number to listen on.
        /// @return True if the server started successfully and is listening, false otherwise.
        bool Initialize(uint16 nPort, std::shared_ptr<Utils::TaskManager> &taskManager);

        /// @brief Starts the server
        /// @details This is a blocking call that runs until Stop() is called.
        void Run();

        /// @brief Stops the server, disconnects all clients, and closes the listen socket.
        void Stop();

        /// @brief Broadcasts a message to all connected clients.
        /// The message is sent reliably.
        /// @param strMessage The message content to broadcast.
        void BroadcastMessage(const std::string &strMessage);

        /// @brief Receives and processes pending messages from all connected clients.
        /// This method should be called regularly to handle incoming data.
        /// In the current implementation, it prints received messages to the console.
        void ReceiveMessages();

    protected:
        /// @brief Handles connection status changes for the server.
        /// Overrides the base class method to manage server-specific connection events,
        /// such as new client connections, disconnections, and connection acceptance.
        /// @param pInfo Pointer to the SteamNetConnectionStatusChangedCallback_t structure.
        virtual void HandleConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t *pInfo) override;

    private:
        /// @brief Handle to the listen socket used by the server.
        /// k_HSteamListenSocket_Invalid if the server is not listening.
        HSteamListenSocket m_hListenSocket;

        /// @brief Vector storing the connection handles of all currently connected clients.
        std::vector<HSteamNetConnection> m_vecClients;

        /// @brief A shared pointer to the task manager.
        /// Used to add tasks to the processing queue
        std::shared_ptr<Utils::TaskManager> m_TaskManager;

        /// @brief Flag indicating whether the ServerManager is currently running.
        bool m_isRunning = false;
    };
} // namespace Core::Net