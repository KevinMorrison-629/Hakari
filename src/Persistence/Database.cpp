#include "Persistence/Database.h"

#include "Utils/Logger.h"
#include <iostream>
#include <unordered_map>

Database::Database(const std::string &dbname)
{
    m_DatabaseName = dbname;
    Utils::Logger::Log(Utils::Logger::Type::INFO, "Using Database: " + m_DatabaseName);
}

Database::~Database()
{
    if (m_DatabaseName.size() > 0)
    {
        Utils::Logger::Log(Utils::Logger::Type::INFO, "Closing Database: " + m_DatabaseName);
    }
}

bool Database::Connect(const std::string &connectionString)
{
    Utils::Logger::Log(Utils::Logger::Type::INFO, "Connecting to MongoDB at " + connectionString);

    mongocxx::uri uri(connectionString);
    m_Client = mongocxx::client(uri);
    m_MongoDB = m_Client["hakari"];

    m_Collection_Card = CollectionWrapper<Card>(m_MongoDB["cards"]);

    return true;
}

Card Database::GetRandomCard()
{
    return m_Collection_Card.getRandom(1)[0];
}

Player Database::GetPlayer(const std::string &discordId)
{
    FieldValue fv;
    fv.type = FieldType::FT_STRING;
    fv.value = discordId;

    std::vector<Player> players;
    players = m_Collection_Player.findByField("discord_id", fv);

    if (players.size() > 0)
    {
        return players[0];
    }
    return Player();
}

void Database::UpdatePlayerEntry(const std::string &id, const std::unordered_map<std::string, FieldValue> &fields)
{
    m_Collection_Player.updateEntryById(id, fields);
}