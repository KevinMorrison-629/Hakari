#include "Backend/ReactionHandler.h"

#include "Utils/Logger.h"

void ReactionHandler::ProcessReaction(dpp::cluster *botRef, const dpp::message_reaction_add_t &event)
{
    dpp::snowflake message_id = event.message_id;
    dpp::snowflake user_id = event.reacting_user.id;
    const dpp::emoji &emoji = event.reacting_emoji;

    if (emoji.id == 0)
    {
        // For a Unicode emoji
        Utils::Logger::Log(Utils::Logger::Type::INFO, "Unicode Emoji: " + emoji.name);
    }
    else
    {
        // For a custom emoji
        dpp::snowflake emoji_id = emoji.id;
        Utils::Logger::Log(Utils::Logger::Type::INFO, "Custom Emoji ID: " + emoji_id.str());
    }
}