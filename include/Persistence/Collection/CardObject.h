#pragma once

#include "Persistence/CollectionEntry.h"
#include <string>

class CardObject : public CollectionEntry
{
public:
    CardObject() = default;

    // Constructor that sets all fields using our generic setters.
    CardObject(const std::string &_id, const std::string &_uuid, const std::string &_cardId, const int32_t &_issueNumber)
    {
        setId(_id);
        setUuid(_uuid);
        setCardId(_cardId);
        setIssueNumber(_issueNumber);
    }

    // Specific getters.
    std::string getId() const { return getValue<std::string>("_id"); }
    std::string getUuid() const { return getValue<std::string>("uuid"); }
    std::string getCardId() const { return getValue<std::string>("card_id"); }
    int32_t getIssueNumber() const { return getValue<int32_t>("issue_number"); }

    // Specific setters.
    void setId(const std::string &id) { setValue<std::string>("_id", FieldType::FT_OBJECT_ID, id); }
    void setUuid(const std::string &uuid) { setValue<std::string>("uuid", FieldType::FT_STRING, uuid); }
    void setCardId(const std::string &cid) { setValue<std::string>("card_id", FieldType::FT_OBJECT_ID, cid); }
    void setIssueNumber(const int32_t &in) { setValue<int32_t>("issue_number", FieldType::FT_INT_32, in); }
};