#pragma once

#include <dpp/dpp.h>

#include "Constants.h"
#include "Persistence/Database.h"

namespace Backend::Commands
{
    /// @brief
    /// @param bot
    /// @param event
    void drop(dpp::cluster *bot, const dpp::slashcommand_t &event, Database &db);

    Constants::CardTier GetRandomCardTier();

} // namespace Backend::Commands
