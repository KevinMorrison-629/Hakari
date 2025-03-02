#include "Backend/InteractionManager.h"
#include "Utils/Logger.h"

#include <ctime>

Database InteractionManager::m_Database("hakari");

InteractionManager::InteractionManager() { m_Database.Connect("mongodb://localhost:27017/"); }

void InteractionManager::MakeValidatedUser(const dpp::snowflake &discord_uid)
{
    if (m_Database.m_Collection_Player.CountEntries({{"uuid", FieldValue(FieldType::FT_STRING, discord_uid.str())}}) == 0)
    {
        // no player entry with discord_id exists, initialize entry
        std::time_t timestamp = std::time(nullptr);

        Player entry;
        entry.setCardCollection({});
        entry.setEssence(100);
        entry.setGems(0);
        entry.setInventory({});
        entry.setLastRollTime(static_cast<int64_t>(timestamp));
        entry.setPrestigeLevel(0);
        entry.setRemainingRolls(10);
        entry.setUuid(discord_uid.str());
        entry.setWishlist({});

        m_Database.m_Collection_Player.Add(entry);

        Utils::Logger::Log(Utils::Logger::Type::DEBUG, "Added Player Entry " + discord_uid.str());
    }
}