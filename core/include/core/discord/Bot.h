#pragma once

#include <dpp/dpp.h>
#include <iostream>

#include "core/utils/TaskManager.h"

namespace Core::Discord
{
    class Bot
    {
    public:
        Bot() = default;
        ~Bot() = default;

        void Initialize(std::shared_ptr<dpp::cluster> &bot, std::shared_ptr<Utils::TaskManager> &taskmanager)
        {
            m_taskManager = taskmanager;

            m_bot = bot;

            // Register event handlers using lambda functions (logging, onReady, slashCommands, etc)
            m_bot->on_log(dpp::utility::cout_logger());

            m_bot->on_ready([this](const dpp::ready_t &event) { this->OnReady(event); });

            m_bot->on_slashcommand([this](const dpp::interaction_create_t &event) { this->OnSlashCommand(event); });

            // m_bot->on_message_reaction_add([this](const dpp::message_reaction_add_t &event)
            //                                { this->OnReactionAdd(event); });

            // m_bot->on_message_reaction_remove([this](const dpp::message_reaction_remove_t &event)
            //                                   { this->OnReactionRemove(event); });
        }

        void Run()
        {
            std::cout << "Bot is starting..." << std::endl;
            m_bot->start(dpp::st_wait);
        };

        void OnReady(const dpp::ready_t &event)
        {
            std::cout << "Bot is online! Logged in as " << m_bot->me.username << std::endl;
        };

        void OnSlashCommand(const dpp::interaction_create_t &event);
        // void OnReactionAdd(const dpp::message_reaction_add_t &event);
        // void OnReactionRemove(const dpp::message_reaction_remove_t &event);

        // void RegisterCommands();
        // void UnregisterCommands();

    private:
        std::shared_ptr<dpp::cluster> m_bot;
        std::shared_ptr<Utils::TaskManager> m_taskManager;
    };
} // namespace Core::Discord