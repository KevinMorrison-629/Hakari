#pragma once

#include "server/core/TaskManager.h"
#include "server/data/DataService.h"
#include <memory>
#include <quicknet/quicknet.h>

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
        void setupRoutes();

        std::shared_ptr<QNET::HttpServer> m_httpServer;
        std::shared_ptr<Core::Utils::TaskManager> m_taskManager;
        std::shared_ptr<Core::Data::DataService> m_dataService;
    };

} // namespace Core::Web