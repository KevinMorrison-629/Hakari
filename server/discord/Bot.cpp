#include "server/discord/Bot.h"
#include "server/tasks/OpenPack.h"

namespace Core::Discord
{
    void Bot::Initialize(std::shared_ptr<dpp::cluster> &bot, std::shared_ptr<Utils::TaskManager> &taskmanager,
                         std::shared_ptr<QDB::Database> &db)
    {
        m_taskManager = taskmanager;
        m_bot = bot;
        m_db = db;

        // Create the single DataService instance for the application.
        m_dataService = std::make_shared<Data::DataService>(m_db);

        m_commandHandler = std::make_shared<Commands::CommandHandler>();
        RegisterCommands();

        m_bot->on_log(dpp::utility::cout_logger());
        m_bot->on_ready([this](const dpp::ready_t &event) { this->OnReady(event); });
        m_bot->on_slashcommand([this](const dpp::interaction_create_t &event) { this->OnSlashCommand(event); });
    }

    void Bot::RegisterCommands()
    {
        m_commandHandler->register_command("drop", Tasks::OpenPackDiscordCommand);
        m_commandHandler->register_command("ping",
                                           [](const Utils::TaskDiscordCommand &task)
                                           {
                                               dpp::message response("Pong!");
                                               task.bot_cluster->interaction_response_edit(task.interaction_token, response);
                                           });
    }

    void Bot::OnSlashCommand(const dpp::interaction_create_t &event)
    {
        event.thinking();
        auto task = std::make_unique<Core::Utils::TaskDiscordCommand>();

        task->type = Core::Utils::TaskType::DPP_SLASH_COMMAND;
        task->priority = Core::Utils::TaskPriority::High;
        task->interaction_token = event.command.token;
        task->command_name = event.command.get_command_name();
        task->user_id = event.command.get_issuing_user().id;

        task->bot_cluster = m_bot;
        task->command_handler = m_commandHandler;
        task->data_service = m_dataService; // Pass the DataService to the task

        m_taskManager->submit(std::move(task));
    }

    void Bot::OnReady(const dpp::ready_t &event)
    {
        std::cout << "Bot is online! Logged in as " << m_bot->me.username << std::endl;
    }
    void Bot::Run()
    {
        std::cout << "Bot is starting..." << std::endl;
        m_bot->start(dpp::st_wait);
    }

} // namespace Core::Discord