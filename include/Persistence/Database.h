#pragma once

#include <string>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include "Constants.h"
#include "Persistence/Collection/Card.h"
#include "Persistence/Collection/CardObject.h"
#include "Persistence/Collection/Character.h"
#include "Persistence/Collection/Player.h"
#include "Persistence/CollectionWrapper.h"

class Database
{
public:
    Database(const std::string &dbname);
    ~Database();

    // Establish a connection to the MongoDB server.
    bool Connect(const std::string &connectionString);

    // Card GetRandomCard();
    // Card GetRandomCardWithFieldValues(const std::unordered_map<std::string, FieldValue> &fields);
    // Card GetAllCardsWithFieldValues(const std::unordered_map<std::string, FieldValue> &fields);
    // bool AddCard(Card);
    // bool UpdateCardEntry(const std::string &id, const std::unordered_map<std::string, FieldValue> &fields);

    // Character GetRandomCharacter();
    // Player GetPlayer(const std::string &discordId);
    // void UpdatePlayerEntry(const std::string &id, const std::unordered_map<std::string, FieldValue> &fields);

    std::string GetRandomUniqueCode(const Constants::UUIDType type);

    CollectionWrapper<Character> m_Collection_Character;
    CollectionWrapper<Card> m_Collection_Card;
    CollectionWrapper<CardObject> m_Collection_CardObject;
    CollectionWrapper<Player> m_Collection_Player;

private:
    std::string m_DatabaseName;

    inline static mongocxx::instance m_Instance{};
    inline static mongocxx::client m_Client;
    inline static mongocxx::v_noabi::database m_MongoDB;
};