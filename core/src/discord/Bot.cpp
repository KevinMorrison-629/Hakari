#include "core/discord/Bot.h"

#include "core/utils/TaskManager.h"

namespace Core::Discord
{
    void Bot::OnSlashCommand(const dpp::interaction_create_t &event)
    {
        event.thinking();

        Core::Utils::TaskDiscordCommand task;
        task.type = Core::Utils::TaskType::DPP_SLASH_COMMAND;
        task.priority = Core::Utils::TaskPriority::High;
        task.bot_cluster = m_bot;
        task.command_name = event.command.get_command_name();
        task.guild_id = event.command.guild_id;
        task.interaction_token = event.command.token;
        task.user_id = event.command.get_issuing_user().id;

        // // get command parameters based on command name
        // if (task.command_name == "ping")
        // {
        //     task.parameters["count"] = std::get<std::string>(event.get_parameter("count"));
        // };

        m_taskManager->submit(task);
    }
}