#include "Backend/CommandHandler.h"

#include <iostream>
#include <sstream>

#include "Persistence/Field.h"
#include "Utils/Logger.h"

void CommandHandler::UnregisterCommands(dpp::cluster *botRef, const dpp::snowflake &guild_id)
{
    // If no test guild id supplied, globally de-register these commands
    // (note: it can take a few hours to propagate across all guilds)
    if (guild_id.empty())
    {
        botRef->global_bulk_command_delete_sync();
    }
    else
    {
        botRef->guild_bulk_command_delete_sync(guild_id);
    }
}

size_t CommandHandler::RegisterCommands(dpp::cluster *botRef, const dpp::snowflake &guild_id)
{
    std::vector<dpp::slashcommand> commands;
    for (SlashCommandEntry entry : m_CommandMap)
    {
        commands.emplace_back(entry.m_name, entry.m_description, botRef->me.id);
    }

    // If no test guild id supplied, globally create these commands
    // (note: it can take a few hours to register all commands across all guilds)
    if (guild_id.empty())
    {
        botRef->global_bulk_command_create_sync(commands);
    }
    else
    {
        botRef->guild_bulk_command_create_sync(commands, guild_id);
    }

    return commands.size();
}

void CommandHandler::ProcessCommand(dpp::cluster *botRef, const dpp::slashcommand_t &event)
{
    std::string commName = event.command.get_command_name();

    for (SlashCommandEntry comm : m_CommandMap)
    {
        if (comm.m_name == commName)
        {
            comm.m_function(botRef, event, m_Database);
        }
    }

    Utils::Logger::Log(Utils::Logger::Type::INFO, "Received Command: " + commName);
}
