#include "Persistence/Database.h"

#include "Utils/Logger.h"
#include "Utils/Utils.h"
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
    m_Collection_CardObject = CollectionWrapper<CardObject>(m_MongoDB["card_objects"]);
    m_Collection_Character = CollectionWrapper<Character>(m_MongoDB["characters"]);
    m_Collection_Player = CollectionWrapper<Player>(m_MongoDB["players"]);

    return true;
}

std::string Database::GetRandomUniqueCode(const Constants::UUIDType type)
{
    uint32_t maxSizeAttempts = 5;
    uint32_t codeLength = 3;

    while (true)
    {
        for (uint32_t attempt = 0; attempt < maxSizeAttempts; attempt++)
        {
            FieldValue testcode;
            testcode.type = FieldType::FT_STRING;
            testcode.value = type.str() + Utils::GenerateRandomCode(codeLength);

            // check if code already exists in database
            bool codeExists = false;
            switch (type.enumerate())
            {
            // case Constants::UUIDTypeEnum::SERVER:
            // case Constants::UUIDTypeEnum::PLAYER:
            case Constants::UUIDTypeEnum::CHARACTER:
            {

                if (m_Collection_Character.CountEntries({{"uuid", testcode}}) > 0)
                {
                    codeExists = true;
                }
                break;
            }
            case Constants::UUIDTypeEnum::CARD:
            {
                if (m_Collection_Card.CountEntries({{"uuid", testcode}}) > 0)
                {
                    codeExists = true;
                }
                break;
            }
            case Constants::UUIDTypeEnum::CARD_OBJECT:
            {
                if (m_Collection_CardObject.CountEntries({{"uuid", testcode}}) > 0)
                {
                    codeExists = true;
                }
                break;
            }
            default:
                return "xx" + Utils::GenerateRandomCode(16); // virtually impossible to get the same code twice. Doesn't
                // really matter since we shouldn't really get here anyways
            }

            if (!codeExists)
            {
                return std::get<std::string>(testcode.value); // Code is unique
            }
        }

        // Increase code length after max_attempts
        maxSizeAttempts *= 2; // Double the attempts for the next length
        codeLength++;         // Increase code length
    }
}