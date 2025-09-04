#pragma once

#include <dpp/dpp.h>
#include <iostream>
#include <memory>

#include "quickdb/quickdb.h"
#include "server/core/CommandHandler.h"
#include "server/core/TaskManager.h"
#include "server/data/DataService.h"

namespace Core::Discord
{
    class Bot
    {
    public:
        Bot() = default;
        ~Bot() = default;

        void Initialize(std::shared_ptr<dpp::cluster> &bot, std::shared_ptr<Utils::TaskManager> &taskmanager,
                        std::shared_ptr<Data::DataService> &db);

        void Run();

    private:
        void OnReady(const dpp::ready_t &event);
        void OnSlashCommand(const dpp::interaction_create_t &event);
        void RegisterCommands();

        std::shared_ptr<dpp::cluster> m_bot;
        std::shared_ptr<Utils::TaskManager> m_taskManager;
        std::shared_ptr<Commands::CommandHandler> m_commandHandler;
        std::shared_ptr<Data::DataService> m_dataService;
    };
} // namespace Core::Discord