#include "Backend/Commands/PingCommand.h"

#include "Utils/Utils.h"

void Backend::Commands::ping(dpp::cluster *bot, const dpp::slashcommand_t &event, Database &db)
{
    dpp::message msg;
    msg.set_content("Pong!");
    event.reply(msg);
}