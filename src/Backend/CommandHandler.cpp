#include "Backend/CommandHandler.h"

#include <sstream>
#include <iostream>

#include "Persistence/Collection/FieldType.h"
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
        botRef->guild_bulk_command_create_sync(commands,
                                               guild_id);
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
            comm.m_function(botRef, event);
        }
    }

    Utils::Logger::Log(Utils::Logger::Type::INFO, "Received Command: " + commName);
}

void CommandHandler::ping(dpp::cluster *bot, const dpp::slashcommand_t &event)
{
}

void CommandHandler::drop(dpp::cluster *bot, const dpp::slashcommand_t &event)
{
    Card card = m_Database.GetRandomCard();

    Utils::Logger::Log(Utils::Logger::Type::INFO, card.toJson());

    // std::unordered_map<std::string, FieldValue> fields;
    // fields["inventory"] = FieldValue(FieldType::FT_ARRAY, {card.getId()});
    // m_Database.UpdatePlayerEntry(event.command.member.user_id.str(), fields);

    dpp::message msg;
    dpp::embed embed;
    embed.title = card.getName();
    embed.description = card.getCharacterId();
    // embed.set_image(card.getImageUrl());
    msg.add_embed(embed);
    event.reply(msg);
}