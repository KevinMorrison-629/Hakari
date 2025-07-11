#include "core/discord/Bot.h"

#include "core/utils/TaskManager.h"

namespace Core::Discord
{
    void Bot::OnSlashCommand(const dpp::interaction_create_t &event)
    {
        /// @todo For now, going to keep this bot "thinking" event. If we expect some commands to quickly resolve,
        /// we can selectively choose to send this command.
        event.thinking();

        std::unique_ptr<Core::Utils::TaskDiscordCommand> task = std::make_unique<Core::Utils::TaskDiscordCommand>();
        task->type = Core::Utils::TaskType::DPP_SLASH_COMMAND;
        task->priority = Core::Utils::TaskPriority::High;
        task->bot_cluster = m_bot;
        task->command_name = event.command.get_command_name();
        task->guild_id = event.command.guild_id;
        task->interaction_token = event.command.token;
        task->user_id = event.command.get_issuing_user().id;

        m_taskManager->submit(std::move(task));
    }
}