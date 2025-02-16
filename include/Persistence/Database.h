#pragma once

#include <string>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include "Persistence/Collection/CollectionWrapper.h"
#include "Persistence/Collection/Card.h"
#include "Persistence/Collection/Player.h"

class Database
{
public:
    Database(const std::string &dbname);
    ~Database();

    // Establish a connection to the MongoDB server.
    bool Connect(const std::string &connectionString);

    Card GetRandomCard();
    Player GetPlayer(const std::string &discordId);
    void UpdatePlayerEntry(const std::string &id, const std::unordered_map<std::string, FieldValue> &fields);

private:
    std::string m_DatabaseName;

    CollectionWrapper<Card> m_Collection_Card;
    CollectionWrapper<Player> m_Collection_Player;

    inline static mongocxx::instance m_Instance{};
    inline static mongocxx::client m_Client;
    inline static mongocxx::v_noabi::database m_MongoDB;
};