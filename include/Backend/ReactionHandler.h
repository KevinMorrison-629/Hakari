#pragma once

#include "Backend/InteractionManager.h"

class ReactionHandler : InteractionManager
{
public:
    ReactionHandler() = default;
    ~ReactionHandler() = default;

    // Process a raw command string received from Discord.
    void ProcessReaction(dpp::cluster *botRef, const dpp::message_reaction_add_t &event);

private:
};