#pragma once

#include <string>
#include "Persistence/Collection/CollectionEntry.h"

class Card : public CollectionEntry
{
public:
    Card() = default;

    // Constructor that sets all fields using our generic setters.
    Card(const std::string &_id,
         const std::string &_name,
         const std::string &_characterId,
         const std::string &_setId,
         const std::string &_tierId,
         const std::string &_imageUrl,
         int32_t _numAcquired)
    {
        setId(_id);
        setName(_name);
        setCharacterId(_characterId);
        setSetId(_setId);
        setTierId(_tierId);
        setImageUrl(_imageUrl);
        setNumAcquired(_numAcquired);
    }

    // Specific getters.
    std::string getId() const { return getValue<std::string>("_id"); }
    std::string getName() const { return getValue<std::string>("name"); }
    std::string getCharacterId() const { return getValue<std::string>("character_id"); }
    std::string getSetId() const { return getValue<std::string>("set_id"); }
    std::string getTierId() const { return getValue<std::string>("tier_id"); }
    std::string getImageUrl() const { return getValue<std::string>("image_url"); }
    int32_t getNumAcquired() const { return getValue<int32_t>("num_acquired", 0); }

    // Specific setters.
    void setId(const std::string &id) { setValue<std::string>("_id", FieldType::FT_OBJECT_ID, id); }
    void setName(const std::string &name) { setValue<std::string>("name", FieldType::FT_STRING, name); }
    void setCharacterId(const std::string &cid) { setValue<std::string>("character_id", FieldType::FT_OBJECT_ID, cid); }
    void setSetId(const std::string &sid) { setValue<std::string>("set_id", FieldType::FT_OBJECT_ID, sid); }
    void setTierId(const std::string &tid) { setValue<std::string>("tier_id", FieldType::FT_OBJECT_ID, tid); }
    void setImageUrl(const std::string &url) { setValue<std::string>("image_url", FieldType::FT_STRING, url); }
    void setNumAcquired(int32_t num) { setValue<int32_t>("num_acquired", FieldType::FT_INT_32, num); }
};