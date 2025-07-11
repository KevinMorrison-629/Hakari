#pragma once

#include <dpp/dpp.h>
#include <iostream>

#include "core/utils/TaskManager.h"

namespace Core::Discord
{
    /// @brief Manages the core functionality of the Discord bot.
    /// @details This class is responsible for initializing the bot, setting up event handlers,
    /// and starting the connection to Discord's gateway.
    class Bot
    {
    public:
        /// @brief Default constructor for the Bot class.
        Bot() = default;

        /// @brief Default destructor for the Bot class.
        ~Bot() = default;

        /// @brief Initializes the bot and its event handlers.
        /// @param bot A shared pointer to the dpp::cluster instance.
        /// @param taskmanager A shared pointer to the TaskManager (Adding processing tasks to queue).
        void Initialize(std::shared_ptr<dpp::cluster> &bot, std::shared_ptr<Utils::TaskManager> &taskmanager)
        {
            m_taskManager = taskmanager;
            m_bot = bot;

            // Setup event listeners
            m_bot->on_log(dpp::utility::cout_logger());
            m_bot->on_ready([this](const dpp::ready_t &event) { this->OnReady(event); });
            m_bot->on_slashcommand([this](const dpp::interaction_create_t &event) { this->OnSlashCommand(event); });
        }

        /// @brief Starts the bot and connects to Discord.
        /// @details This is a blocking call that will run until the bot is shut down.
        void Run()
        {
            std::cout << "Bot is starting..." << std::endl;
            m_bot->start(dpp::st_wait);
        };

        /// @brief Handles the 'ready' event from Discord.
        /// @details This function is called once the bot has successfully connected to Discord's gateway.
        /// @param event The ready event data.
        void OnReady(const dpp::ready_t &event)
        {
            std::cout << "Bot is online! Logged in as " << m_bot->me.username << std::endl;
        };

        /// @brief Handles incoming slash command interactions.
        /// @param event The interaction create event data.
        void OnSlashCommand(const dpp::interaction_create_t &event);

    private:
        /// @brief A shared pointer to the main dpp::cluster object.
        std::shared_ptr<dpp::cluster> m_bot;

        /// @brief A shared pointer to the task manager.
        /// Used to add tasks to the processing queue
        std::shared_ptr<Utils::TaskManager> m_taskManager;
    };
} // namespace Core::Discord