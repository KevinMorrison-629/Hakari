#pragma once

#include <quicknet/quicknet.h>

#include "server/core/TaskManager.h"
#include "server/data/DataService.h"
#include <memory>

namespace Core::Web
{

    class WebService
    {
    public:
        WebService(std::shared_ptr<Core::Utils::TaskManager> &taskManager,
                   std::shared_ptr<Core::Data::DataService> &dataService);

        void Initialize(uint16_t port);
        void Run();
        void Stop();

    private:
        bool checkFrontendDirectory(const std::string &frontendDir);
        void setupRoutes(const std::string &frontend_dir);

        std::shared_ptr<QNET::HttpServer> m_httpServer;
        std::shared_ptr<Core::Utils::TaskManager> m_taskManager;
        std::shared_ptr<Core::Data::DataService> m_dataService;

        uint16_t m_port;
    };

} // namespace Core::Web