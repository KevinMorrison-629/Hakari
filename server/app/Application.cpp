#include "server/app/Application.h"

namespace Server
{
    void Application::Initialize(int32_t server_port, const std::string &bot_token)
    {

        // Connect to Database
        Core::Data::Database::instance().Connect("mongodb://localhost:27017/?maxPoolSize=50", "hakaridb");

        // Instantiate Task Manager
        m_TaskManager = std::make_shared<Core::Utils::TaskManager>(4);
        if (!m_TaskManager)
        {
            std::cerr << "Failed to start task manager." << std::endl;
        }

        // Instantiate Server Connection
        m_ConnectionManager = std::make_shared<Core::Net::ServerManager>();
        if (!m_ConnectionManager->Initialize(9000, m_TaskManager))
        {
            std::cerr << "Failed to start server." << std::endl;
        }

        // Initiate Discord Bot
        m_cluster = std::make_shared<dpp::cluster>(bot_token, dpp::i_default_intents | dpp::i_guild_messages);
        m_DiscordManager = std::make_shared<Core::Discord::Bot>();
        m_DiscordManager->Initialize(m_cluster, m_TaskManager);

        m_isRunning = true;
    }

    void Application::Start()
    {
        std::thread discordManagerThread(&Core::Discord::Bot::Run, m_DiscordManager);
        std::thread connectionManagerThread(&Core::Net::ServerManager::Run, m_ConnectionManager);

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

}