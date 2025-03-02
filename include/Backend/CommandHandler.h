#pragma once

#include "Backend/InteractionManager.h"

#include "Backend/Commands/DropCommand.h"
#include "Backend/Commands/PingCommand.h"

class CommandHandler : InteractionManager
{
public:
    /// @brief Deregister all Commands with the Bot
    /// @param botRef pointer reference to the bot cluster
    /// @param guild_id reference to the guild in which to de-register
    void UnregisterCommands(dpp::cluster *botRef, const dpp::snowflake &guild_id);

    // Register a command and its associated handler.
    size_t RegisterCommands(dpp::cluster *botRef, const dpp::snowflake &guild_id);

    // Process a raw command string received from Discord.
    void ProcessCommand(dpp::cluster *botRef, const dpp::slashcommand_t &event);

private:
    struct SlashCommandEntry
    {
        SlashCommandEntry(
            const std::string &name,
            const std::function<void(dpp::cluster *bot, const dpp::slashcommand_t &event, Database &db)> &function,
            const std::string &description)
        {
            m_name = name;
            m_function = function;
            m_description = description;
        }

        std::string m_name;
        std::string m_description;
        std::function<void(dpp::cluster *bot, const dpp::slashcommand_t &event, Database &db)> m_function;
    };

    // static void ping(dpp::cluster *bot, const dpp::slashcommand_t &event);
    // static void verify(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void vote(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void daily(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void drop(dpp::cluster *bot, const dpp::slashcommand_t &event);
    // static void collection(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void view(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void search(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void wishlist(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void wishadd(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void wishremove(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void burn(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void give(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void trade(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void inventory(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void itemshop(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void gemshop(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void useitem(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void userinfo(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void serverinfo(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void chestview(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void chestgive(dpp::cluster &bot, const dpp::slashcommand_t &event);
    // static void gambleadd(dpp::cluster &bot, const dpp::slashcommand_t &event);

    // Map of command names to their handler functions.
    std::vector<SlashCommandEntry> m_CommandMap = {
        SlashCommandEntry("ping", Backend::Commands::ping, "Send Message and Ask for Response"),
        // SlashCommandEntry("verify", verify, "Verify account. \"Do you want to play a game?\""),
        // SlashCommandEntry("vote", vote, "Vote for Hakaribot! (we might bribe you with some gemstones. . .)"),
        // SlashCommandEntry("daily", daily, "Here's your daily allowance (50-150 essence)"),
        SlashCommandEntry("drop", Backend::Commands::drop, "Drop a set of cards here. Get em Fast!"),
        // SlashCommandEntry("collection", collection, "Let's take a look at that collection. IS THAT A LEGENDARY???"),
        // SlashCommandEntry("view", view, "Let me take a better look at that one card . ."),
        // SlashCommandEntry("search", search, "Look at the stats for a particular character/set/series/movie/.../whatever"),
        // SlashCommandEntry("wishlist", wishlist, "Who do you *really* want?"),
        // SlashCommandEntry("wishadd", wishadd, "Add Wish. I *really* want this character"),
        // SlashCommandEntry("wishremove", wishremove, "Remove Wish. \"I don't want to play with you anymore\" - Andy?"),
        // SlashCommandEntry("burn", burn, "Destroy a card for it's resources. Some people just want to see the world burn"),
        // SlashCommandEntry("give", give, "Give an item to another player. Charity."),
        // SlashCommandEntry("trade", trade, "Make a trade with another player. \"!TRADE OFFER!: I get ___. You Receive
        // ___.\""), SlashCommandEntry("inventory", inventory, "Show your inventory. \"What have you gots in your
        // pocketses\""), SlashCommandEntry("itemshop", itemshop, "Display the Item Shop. \"I'll take one of those
        // *points*\""), SlashCommandEntry("gemshop", gemshop, "Display the Gem Shop. \"I've been saving up years for this
        // upgrade!\""), SlashCommandEntry("useitem", useitem, "Use an item from your inventory. \"What does this do
        // again?\""), SlashCommandEntry("userinfo", userinfo, "Display some user info/stats. Let's see if you are a
        // degenerate. . ."), SlashCommandEntry("serverinfo", serverinfo, "Display some server info/stats."),
        // SlashCommandEntry("chestview", chestview, "View the inventory for the server. Teamwork Makes the Dream Work."),
        // SlashCommandEntry("chestgive", chestgive, "Contribute to the server chest."),
        // SlashCommandEntry("gamebleadd", gambleadd, "99% of Gamblers Quit Before They Make It Big!"),
    };
};