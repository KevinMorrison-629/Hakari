
#include <memory>
#include <string>
#include <thread>

#include <dpp/dpp.h>

#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

#include "core/data/Database.h"
#include "core/net/ServerManager.h"
#include "core/utils/TaskManager.h"

#include "core/discord/Bot.h"

namespace Server
{
    // forward declarations (remove once implemented)
    // class DiscordManager;

    class Application
    {
    public:
        Application() = default;
        ~Application() = default;

        void Initialize(int32_t server_port, const std::string &bot_token);
        void Start();
        void Shutdown();

    private:
        bool m_isRunning = false;

        std::shared_ptr<Core::Net::ServerManager> m_ConnectionManager;
        std::shared_ptr<Core::Discord::Bot> m_DiscordManager;

        std::shared_ptr<Core::Utils::TaskManager> m_TaskManager;

        std::shared_ptr<dpp::cluster> m_cluster;
    };
} // namespace Server