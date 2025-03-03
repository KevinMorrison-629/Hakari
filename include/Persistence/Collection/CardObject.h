#pragma once

#include "Persistence/CollectionEntry.h"
#include <string>

class CardObject : public CollectionEntry
{
public:
    CardObject() = default;

    // Constructor that sets all fields using our generic setters.
    CardObject(const std::string &_id, const std::string &_uuid, const std::string &_cardId, const std::string &_name,
               const int32_t &_issueNumber, const int32_t &_starNumber, const int32_t &_expectedValue,
               const std::string &_playerUuid)
    {
        setId(_id);
        setUuid(_uuid);
        setCardId(_cardId);
        setName(_name);
        setIssueNumber(_issueNumber);
        setStarNumber(_starNumber);
        setExpectedValue(_expectedValue);
        setOwningPlayerUuid(_playerUuid);
    }

    // Specific getters.
    std::string getId() const { return getValue<std::string>("_id"); }
    std::string getUuid() const { return getValue<std::string>("uuid"); }
    std::string getCardId() const { return getValue<std::string>("card_id"); }
    std::string getName() const { return getValue<std::string>("name"); }
    int32_t getIssueNumber() const { return getValue<int32_t>("issue_number"); }
    int32_t getStarNumber() const { return getValue<int32_t>("star_number"); }
    int32_t getExpectedValue() const { return getValue<int32_t>("expected_value"); }
    std::string getOwningPlayerUuid() const { return getValue<std::string>("player_uuid"); }

    // Specific setters.
    void setId(const std::string &id) { setValue<std::string>("_id", FieldType::FT_OBJECT_ID, id); }
    void setUuid(const std::string &uuid) { setValue<std::string>("uuid", FieldType::FT_STRING, uuid); }
    void setCardId(const std::string &cid) { setValue<std::string>("card_id", FieldType::FT_OBJECT_ID, cid); }
    void setName(const std::string &name) { setValue<std::string>("name", FieldType::FT_STRING, name); }
    void setIssueNumber(const int32_t &in) { setValue<int32_t>("issue_number", FieldType::FT_INT_32, in); }
    void setStarNumber(const int32_t &star) { setValue<int32_t>("star_number", FieldType::FT_INT_32, star); }
    void setExpectedValue(const int32_t &eval) { setValue<int32_t>("expected_value", FieldType::FT_INT_32, eval); }
    void setOwningPlayerUuid(const std::string &pid) { setValue<std::string>("player_uuid", FieldType::FT_STRING, pid); }
};