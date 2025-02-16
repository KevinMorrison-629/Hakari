
#include "HakariBot.h"

#include "Utils/Utils.h"

// Constructor: stores the bot token and logs the creation.
HakariBot::HakariBot(dpp::cluster *botRef, const std::map<std::string, uint64_t> &arguments)
{
    // Set the reference to the Bot Cluster
    m_bot = botRef;

    // Handle DPP Logging (send to cout logger)
    m_bot->on_log(dpp::utility::cout_logger());

    // Set Initial "Presence" for the Bot
    m_bot->set_presence(dpp::presence(dpp::ps_online, dpp::at_custom, "Gambling Away My Life Savings"));
    Utils::Logger::Log(Utils::Logger::Type::INFO, "DiscordBot created with token: " + botRef->token);

    // Register Commands (if --register)
    if (arguments.find("--register") != arguments.end())
    {
        // Initialize Bot Slash Commands
        m_bot->on_ready([this](const dpp::ready_t &event)
                        {
                            UnregisterCommands(dpp::snowflake(1215786056922824715));
                            RegisterCommands(std::move(dpp::snowflake(1215786056922824715))); // Register test server
                        });
    }
    else if (arguments.find("--registerAllGuilds") != arguments.end())
    {
        // Initialize Bot Slash Commands
        m_bot->on_ready([this](const dpp::ready_t &event)
                        {
                            UnregisterCommands(dpp::snowflake(0));
                            RegisterCommands(dpp::snowflake(0)); // Register all guilds - send empty test guild
                        });
    }

    // Log Active Guilds
    if (arguments.find("--logActiveGuilds") != arguments.end())
    {
        // Get All Active Guilds that Bot is in and log to file
        dpp::guild_map guildmap = m_bot->current_user_get_guilds_sync();

        std::vector<std::string> guilds;
        for (auto guild : guildmap)
        {
            std::string entry = guild.first.str() + " - " + guild.second.name + " [members: " + std::to_string(guild.second.member_count) + "]";
            guilds.push_back(entry);
        }
        Utils::LogActiveGuilds(guilds);
    }

    // Define Handler for Slash Commands
    m_bot->on_slashcommand([this](const dpp::slashcommand_t &command_event)
                           {
                            const dpp::slashcommand_t command_copy = command_event; 
                            m_CommandHandler.ProcessCommand(m_bot, command_copy); });

    // Define Handler for ReactionAdd Events
    m_bot->on_message_reaction_add([this](const dpp::message_reaction_add_t &reaction_event)
                                   {
                                    const dpp::message_reaction_add_t reaction_copy = reaction_event;
                                    m_ReactionHandler.ProcessReaction(m_bot, reaction_copy); });
}

// Destructor: perform any necessary cleanup.
HakariBot::~HakariBot()
{
    Utils::Logger::Log(Utils::Logger::Type::INFO, "DiscordBot Shutting Down. . .");
}

// Connect to Discord (simulation).
void HakariBot::Start()
{
    Utils::Logger::Log(Utils::Logger::Type::INFO, "Connecting to Discord with token: " + m_bot->token);

    m_bot->start(dpp::st_wait);
}

// Setup event listeners (simulation).
void HakariBot::RegisterCommands(const dpp::snowflake &guild_id)
{
    Utils::Logger::Log(Utils::Logger::Type::INFO, "Registering Discord Commands...");

    size_t numRegisteredCommands = m_CommandHandler.RegisterCommands(m_bot, guild_id);

    Utils::Logger::Log(Utils::Logger::Type::INFO, "Registered " + std::to_string(numRegisteredCommands) + " Discord Commands.");
}

void HakariBot::UnregisterCommands(const dpp::snowflake &guild_id)
{
    Utils::Logger::Log(Utils::Logger::Type::INFO, "Unregistering Discord Commands...");

    m_CommandHandler.UnregisterCommands(m_bot, guild_id);

    Utils::Logger::Log(Utils::Logger::Type::INFO, "Unregistered Discord Commands.");
}