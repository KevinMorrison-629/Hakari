#pragma once

#include "core/data/CollectionEntry.h"
#include <string>

namespace Core::Data::Collection
{
    class Card : public CollectionEntry
    {
    public:
        Card() = default;

        // Constructor that sets all fields using our generic setters.
        Card(const bsoncxx::oid &_id, const std::string &_uuid, const std::string &_name, const bsoncxx::oid &_characterId,
             const bsoncxx::oid &_setId, const int32_t &_tier, const std::string &_imageUrl, int32_t _numAcquired)
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
        bsoncxx::oid getId() const { return getValue<bsoncxx::oid>("_id"); }
        std::string getUuid() const { return getValue<std::string>("uuid"); }
        std::string getName() const { return getValue<std::string>("name"); }
        bsoncxx::oid getCharacterId() const { return getValue<bsoncxx::oid>("character_id"); }
        bsoncxx::oid getSetId() const { return getValue<bsoncxx::oid>("set_id"); }
        int32_t getTierId() const { return getValue<int32_t>("tier"); }
        std::string getImageUrl() const { return getValue<std::string>("image_url"); }
        int32_t getNumAcquired() const { return getValue<int32_t>("num_acquired", 0); }

        // Specific setters.
        void setId(const bsoncxx::oid &id) { setValue("_id", id); }
        void setUuid(const std::string &uuid) { setValue("uuid", uuid); }
        void setName(const std::string &name) { setValue("name", name); }
        void setCharacterId(const bsoncxx::oid &cid) { setValue("character_id", cid); }
        void setSetId(const bsoncxx::oid &sid) { setValue("set_id", sid); }
        void setTier(const int32_t &tier) { setValue("tier", tier); }
        void setImageUrl(const std::string &url) { setValue("image_url", url); }
        void setNumAcquired(int32_t num) { setValue("num_acquired", num); }
    };
}