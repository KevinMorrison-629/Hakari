#pragma once

#include <dpp/dpp.h>

#include "Constants.h"
#include "Persistence/Database.h"

namespace Backend::Commands
{
    /// @brief
    /// @param bot
    /// @param event
    void ping(dpp::cluster *bot, const dpp::slashcommand_t &event, Database &db);

} // namespace Backend::Commands
