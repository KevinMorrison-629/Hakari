#pragma once

#include <string>

#include <dpp/dpp.h>

#include "Backend/CommandHandler.h"
#include "Backend/ReactionHandler.h"
#include "Utils/Logger.h"

// This class interfaces with Discord++ to handle events and commands.
class HakariBot
{
public:
    HakariBot(dpp::cluster *botRef, const std::map<std::string, uint64_t> &arguments);
    ~HakariBot();

    void Start();
    void Shutdown();

    void RegisterCommands(const dpp::snowflake &guild_id);
    void UnregisterCommands(const dpp::snowflake &guild_id);

    // void HandleSlashCommand(const dpp::slashcommand_t &command_event);
    // void HandleReactionAdd(const dpp::message_reaction_add_t &reaction_event);
    // void HandleMenuSelect(const dpp::select_click_t &select_event);
    // void HandleButtonEvent(const dpp::button_click_t &button_event);

private:
    dpp::cluster *m_bot;

    CommandHandler m_CommandHandler;   // Used to parse and dispatch commands.
    ReactionHandler m_ReactionHandler; // Used to parse and dispatch events based on reactions
};