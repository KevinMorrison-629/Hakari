
#include <string>
#include <memory>
#include <thread>

#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

#include "core/data/Database.h"
#include "core/net/ServerManager.h"
#include "core/utils/TaskManager.h"

namespace Server
{
    // forward declarations (remove once implemented)
    // class DiscordManager;

    class Application
    {
    public:
        Application() = default;
        ~Application() = default;

        void Initialize(int32_t server_port);
        void Run();
        void Shutdown();

    private:
        bool m_isRunning = false;

        std::unique_ptr<Core::Net::ServerManager> m_ConnectionManager;
        // std::unique_ptr<DiscordManager> m_DiscordManager;

        std::unique_ptr<Core::Utils::TaskManager> m_TaskManager;
    };
}