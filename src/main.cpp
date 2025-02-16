
#include <iostream>

#include "HakariBot.h"
#include "Utils/Utils.h"
#include "Utils/Logger.h"

#include <dpp/dpp.h>

int main(int argc, char *argv[])
{
    std::map<std::string, uint64_t> arguments;
    for (size_t i = 0; i < argc; i++)
    {
        arguments[argv[i]] = i;
    }

    Utils::Logger::Log(Utils::Logger::Type::INFO, "Starting HakariBot. . .\n\n");

    // Load the Bot Token
    const std::string TOKEN_FILENAME = "bot_token.txt";
    std::string BOT_TOKEN;
    Utils::ErrorCode ec = Utils::LoadToken(TOKEN_FILENAME, BOT_TOKEN);

    if (ec != Utils::ErrorCode::SUCCESS)
    {
        Utils::Logger::Log(Utils::Logger::Type::FATAL_ERROR, "Could Not Load Bot Token");
        return 0;
    }

    // Initialize the Bot
    dpp::cluster cluster(BOT_TOKEN);
    HakariBot bot = HakariBot(&cluster, arguments);

    // Start the Bot
    bot.Start();
    Utils::Logger::Log(Utils::Logger::Type::WARNING, "Bot Shutting Down. Event Loop Ended");

    return 0;
}