
#include <memory>
#include <string>
#include <thread>

#include "quickdb/quickdb.h"
#include "quicknet/quicknet.h"

#include <dpp/dpp.h>

#include "server/core/TaskManager.h"
#include "server/discord/Bot.h"

namespace Server
{
    /// @brief The main class that encapsulates the entire server application logic.
    /// @details This class is responsible for initializing, running, and shutting down all major
    /// components, including the network server, the Discord bot, and the task manager.
    class Application
    {
    public:
        /// @brief Default constructor for the Application class.
        Application() = default;

        /// @brief Default destructor for the Application class.
        ~Application() = default;

        /// @brief Initializes all the major components of the application.
        /// @param server_port The port number for the network server to listen on.
        /// @param bot_token The authentication token for the Discord bot.
        void Initialize(int32_t server_port, const std::string &bot_token);

        /// @brief Starts the application main loop and all services.
        /// @details This is a blocking call that runs until Shutdown() is called.
        void Start();

        /// @brief Shuts down all services and cleans up resources.
        void Shutdown();

    public:
        void ProcessMessage(HSteamNetConnection hConn, const std::vector<uint8_t> &byteMsg);

    private:
        bool m_isRunning = false; ///< @brief Flag indicating whether the application is currently running.

        std::shared_ptr<QNET::Server> m_ConnectionManager; ///< @brief Manages network connections and communication.
        std::shared_ptr<QDB::Database> m_Database;
        std::shared_ptr<Core::Discord::Bot>
            m_DiscordManager; ///< @brief Manages the Discord bot's connection and event handling.
        std::shared_ptr<Core::Utils::TaskManager>
            m_TaskManager;                       ///< @brief Manages the thread pool for processing asynchronous tasks.
        std::shared_ptr<dpp::cluster> m_cluster; ///< @brief The dpp::cluster object for interacting with the Discord API.
    };
} // namespace Server