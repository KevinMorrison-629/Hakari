#pragma once

#include "Persistence/CollectionEntry.h"
#include <string>

class Card : public CollectionEntry
{
public:
    Card() = default;

    // Constructor that sets all fields using our generic setters.
    Card(const std::string &_id, const std::string &_uuid, const std::string &_name, const std::string &_characterId,
         const std::string &_setId, const int32_t &_tier, const std::string &_imageUrl, int32_t _numAcquired)
    {
        setId(_id);
        setUuid(_uuid);
        setName(_name);
        setCharacterId(_characterId);
        setSetId(_setId);
        setTier(_tier);
        setImageUrl(_imageUrl);
        setNumAcquired(_numAcquired);
    }

    // Specific getters.
    std::string getId() const { return getValue<std::string>("_id"); }
    std::string getUuid() const { return getValue<std::string>("uuid"); }
    std::string getName() const { return getValue<std::string>("name"); }
    std::string getCharacterId() const { return getValue<std::string>("character_id"); }
    std::string getSetId() const { return getValue<std::string>("set_id"); }
    int32_t getTierId() const { return getValue<int32_t>("tier"); }
    std::string getImageUrl() const { return getValue<std::string>("image_url"); }
    int32_t getNumAcquired() const { return getValue<int32_t>("num_acquired", 0); }

    // Specific setters.
    void setId(const std::string &id) { setValue<std::string>("_id", FieldType::FT_OBJECT_ID, id); }
    void setUuid(const std::string &uuid) { setValue<std::string>("uuid", FieldType::FT_STRING, uuid); }
    void setName(const std::string &name) { setValue<std::string>("name", FieldType::FT_STRING, name); }
    void setCharacterId(const std::string &cid) { setValue<std::string>("character_id", FieldType::FT_OBJECT_ID, cid); }
    void setSetId(const std::string &sid) { setValue<std::string>("set_id", FieldType::FT_OBJECT_ID, sid); }
    void setTier(const int32_t &tier) { setValue<int32_t>("tier", FieldType::FT_INT_32, tier); }
    void setImageUrl(const std::string &url) { setValue<std::string>("image_url", FieldType::FT_STRING, url); }
    void setNumAcquired(int32_t num) { setValue<int32_t>("num_acquired", FieldType::FT_INT_32, num); }
};