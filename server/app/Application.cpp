#include "server/app/Application.h"

#include "common/data/Dataclasses.h"
#include "common/net/protocol.h"

#include <sodium.h>

namespace Server
{
    void Application::Initialize(int32_t server_port, uint16_t http_port, const std::string &bot_token)
    {

        // It's crucial to initialize libsodium before any cryptographic functions are used.
        if (sodium_init() < 0)
        {
            throw std::runtime_error("Failed to initialize libsodium!");
        }

        // Connect to Database
        m_Database = std::make_shared<QDB::Database>("mongodb://localhost:27017/?maxPoolSize=50");

        // Create a single, shared DataService instance that all other services will use.
        auto dataService = std::make_shared<Core::Data::DataService>(m_Database);

        // Instantiate Task Manager
        m_TaskManager = std::make_shared<Core::Utils::TaskManager>(4);
        if (!m_TaskManager)
        {
            std::cerr << "Failed to start task manager." << std::endl;
        }

        // Instantiate Server Connection
        m_ConnectionManager = std::make_shared<QNET::Server>();
        if (m_ConnectionManager->Initialize(9000))
        {
            m_ConnectionManager->OnMessageReceived = [this](HSteamNetConnection hConn, const std::vector<uint8_t> &byteMsg)
            { this->ProcessMessage(hConn, byteMsg); };
        }
        else
        {
            std::cerr << "Failed to start server." << std::endl;
        }

        // Instantiate Web Service for web clients
        m_WebService = std::make_shared<Core::Web::WebService>(m_TaskManager, dataService);
        m_WebService->Initialize(http_port);

        // Initiate Discord Bot
        m_cluster = std::make_shared<dpp::cluster>(bot_token, dpp::i_default_intents | dpp::i_guild_messages);
        m_DiscordManager = std::make_shared<Core::Discord::Bot>();
        m_DiscordManager->Initialize(m_cluster, m_TaskManager, dataService);

        m_isRunning = true;
    }

    void Application::Start()
    {
        std::thread discordManagerThread(&Core::Discord::Bot::Run, m_DiscordManager);
        std::thread connectionManagerThread(&QNET::Server::Run, m_ConnectionManager);

        discordManagerThread.join();
        connectionManagerThread.join();
    }

    void Application::Shutdown()
    {
        m_isRunning = false;

        // Shutdown Connection Manager (disable receiving messages)
        if (m_ConnectionManager)
        {
            m_ConnectionManager->Stop();
        }
    }

    void Application::ProcessMessage(HSteamNetConnection hConn, const std::vector<uint8_t> &byteMsg)
    {
        Core::Net::MessageType type = Core::Net::PeekMessageType(byteMsg);

        if (static_cast<uint8_t>(type) > 127U)
        {
            // Time Deferrable
            switch (type)
            {
            }

            // m_TaskManager->submit();
        }
        else
        {
        }
    }

} // namespace Server