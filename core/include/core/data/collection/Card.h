#pragma once

#include "core/data/CollectionEntry.h"
#include <string>

namespace Core::Data::Collection
{
    /// @brief Represents a Card document in a collection.
    /// Inherits from CollectionEntry to provide common document handling capabilities
    /// and adds specific getters and setters for card-related fields.
    class Card : public CollectionEntry
    {
    public:
        /// @brief Default constructor.
        Card() = default;

        /// @brief Constructs a Card object and initializes its fields.
        /// @param _id The MongoDB ObjectId of the card.
        /// @param _uuid A unique string identifier for the card.
        /// @param _name The name of the card.
        /// @param _characterId The ObjectId of the character associated with this card.
        /// @param _setId The ObjectId of the set this card belongs to.
        /// @param _tier The tier or rarity of the card.
        /// @param _imageUrl The URL for the card's image.
        /// @param _numAcquired The number of times this card has been acquired by a player (if applicable in context).
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

        // Specific getters
        /// @brief Gets the MongoDB ObjectId of the card.
        /// @return The card's ObjectId.
        bsoncxx::oid getId() const { return getValue<bsoncxx::oid>("_id"); }
        /// @brief Gets the unique string identifier (UUID) of the card.
        /// @return The card's UUID string.
        std::string getUuid() const { return getValue<std::string>("uuid"); }
        /// @brief Gets the name of the card.
        /// @return The card's name.
        std::string getName() const { return getValue<std::string>("name"); }
        /// @brief Gets the ObjectId of the character associated with this card.
        /// @return The character's ObjectId.
        bsoncxx::oid getCharacterId() const { return getValue<bsoncxx::oid>("character_id"); }
        /// @brief Gets the ObjectId of the set this card belongs to.
        /// @return The set's ObjectId.
        bsoncxx::oid getSetId() const { return getValue<bsoncxx::oid>("set_id"); }
        /// @brief Gets the tier or rarity of the card.
        /// @return The card's tier as an integer.
        int32_t getTierId() const { return getValue<int32_t>("tier"); }
        /// @brief Gets the URL for the card's image.
        /// @return The image URL string.
        std::string getImageUrl() const { return getValue<std::string>("image_url"); }
        /// @brief Gets the number of times this card has been acquired.
        /// Defaults to 0 if the field is not present.
        /// @return The number of times acquired.
        int32_t getNumAcquired() const { return getValue<int32_t>("num_acquired", 0); }

        // Specific setters
        /// @brief Sets the MongoDB ObjectId of the card.
        /// @param id The ObjectId to set.
        void setId(const bsoncxx::oid &id) { setValue("_id", id); }
        /// @brief Sets the unique string identifier (UUID) of the card.
        /// @param uuid The UUID string to set.
        void setUuid(const std::string &uuid) { setValue("uuid", uuid); }
        /// @brief Sets the name of the card.
        /// @param name The name string to set.
        void setName(const std::string &name) { setValue("name", name); }
        /// @brief Sets the ObjectId of the character associated with this card.
        /// @param cid The character's ObjectId to set.
        void setCharacterId(const bsoncxx::oid &cid) { setValue("character_id", cid); }
        /// @brief Sets the ObjectId of the set this card belongs to.
        /// @param sid The set's ObjectId to set.
        void setSetId(const bsoncxx::oid &sid) { setValue("set_id", sid); }
        /// @brief Sets the tier or rarity of the card.
        /// @param tier The tier integer to set.
        void setTier(const int32_t &tier) { setValue("tier", tier); }
        /// @brief Sets the URL for the card's image.
        /// @param url The image URL string to set.
        void setImageUrl(const std::string &url) { setValue("image_url", url); }
        /// @brief Sets the number of times this card has been acquired.
        /// @param num The number of times acquired.
        void setNumAcquired(int32_t num) { setValue("num_acquired", num); }
    };
}