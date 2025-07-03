#include "server/Application.h"

namespace Server
{
    void Application::Initialize(int32_t server_port)
    {

        // Connect to Database
        Core::Data::Database::instance().Connect("mongodb://localhost:27017/?maxPoolSize=50", "hakaridb");

        // Instantiate Server Connection
        m_ConnectionManager = std::make_unique<Core::Net::ServerManager>();
        if (!m_ConnectionManager->Start(9000))
        {
            std::cerr << "Failed to start server." << std::endl;
        }

        // Instantiate Task Manager
        m_TaskManager = std::make_unique<Core::Utils::TaskManager>(4);
        if (!m_TaskManager)
        {
            std::cerr << "Failed to start task manager." << std::endl;
        }

        m_isRunning = true;
    }

    void Application::Run()
    {
        while (m_isRunning)
        {
            m_ConnectionManager->Poll();
            m_ConnectionManager->ReceiveMessages(m_TaskManager);

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
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