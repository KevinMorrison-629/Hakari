#pragma once

#include "Persistence/Collection/CollectionEntry.h"
#include <string>
#include <vector>

class Player : public CollectionEntry
{
public:
    Player() = default;

    // Constructor to initialize all fields
    Player(const std::string &_id, const std::string &_discordId, const std::string &_avatarUrl, int64_t _essence,
           int64_t _gems, const std::vector<std::string> &_inventory, const std::vector<std::string> &_wishlist,
           int32_t _remainingRolls, int64_t _lastRollTime, const std::vector<std::string> &_cardCollection,
           int32_t _prestigeLevel)
    {
        setId(_id);
        setDiscordId(_discordId);
        setAvatarUrl(_avatarUrl);
        setEssence(_essence);
        setGems(_gems);
        setInventory(_inventory);
        setWishlist(_wishlist);
        setRemainingRolls(_remainingRolls);
        setLastRollTime(_lastRollTime);
        setCardCollection(_cardCollection);
        setPrestigeLevel(_prestigeLevel);
    }

    // Specific Getters
    std::string getId() const { return getValue<std::string>("_id"); }
    std::string GetDiscordId() const { return getValue<std::string>("discord_id"); }
    std::string getAvatarUrl() const { return getValue<std::string>("avatar_url"); }
    int64_t getEssence() const { return getValue<int64_t>("essence", 0); }
    int64_t getGems() const { return getValue<int64_t>("gems", 0); }
    int32_t getRemainingRolls() const { return getValue<int32_t>("remaining_rolls", 0); }
    int64_t getLastRollTime() const { return getValue<int64_t>("last_roll_time", 0); }
    int32_t getPrestigeLevel() const { return getValue<int32_t>("prestige_level", 0); }
    std::vector<std::string> getInventory() const { return getObjectIdArray("inventory"); }
    std::vector<std::string> getWishlist() const { return getObjectIdArray("wishlist"); }
    std::vector<std::string> getCardCollection() const { return getObjectIdArray("card_collection"); }

    // Specific Setters
    void setId(const std::string &id) { setValue<std::string>("_id", FieldType::FT_OBJECT_ID, id); }
    void setDiscordId(const std::string &did) { setValue<std::string>("discord_id", FieldType::FT_STRING, did); }
    void setAvatarUrl(const std::string &url) { setValue<std::string>("avatar_url", FieldType::FT_STRING, url); }
    void setEssence(int64_t essence) { setValue<int64_t>("essence", FieldType::FT_INT_64, essence); }
    void setGems(int64_t gems) { setValue<int64_t>("gems", FieldType::FT_INT_64, gems); }
    void setRemainingRolls(int32_t rolls) { setValue<int32_t>("remaining_rolls", FieldType::FT_INT_32, rolls); }
    void setLastRollTime(int64_t timestamp) { setValue<int64_t>("last_roll_time", FieldType::FT_TIMESTAMP, timestamp); }
    void setPrestigeLevel(int32_t level) { setValue<int32_t>("prestige_level", FieldType::FT_INT_32, level); }

    void setInventory(const std::vector<std::string> &inventory) { setObjectIdArray("inventory", inventory); }
    void setWishlist(const std::vector<std::string> &wishlist) { setObjectIdArray("wishlist", wishlist); }
    void setCardCollection(const std::vector<std::string> &collection) { setObjectIdArray("card_collection", collection); }

private:
    // Helper function to get an array of ObjectIds as strings
    std::vector<std::string> getObjectIdArray(const std::string &field) const
    {
        std::vector<std::string> result;
        if (auto f = getField(field))
        {
            auto &arrayValues = std::get<std::vector<FieldValue>>(f->value);
            for (const auto &val : arrayValues)
            {
                result.push_back(std::get<std::string>(val.value));
            }
        }
        return result;
    }

    // Helper function to set an array of ObjectIds
    void setObjectIdArray(const std::string &field, const std::vector<std::string> &values)
    {
        std::vector<FieldValue> fieldValues;
        for (const auto &val : values)
        {
            fieldValues.push_back(FieldValue{FieldType::FT_OBJECT_ID, val});
        }
        setField(field, FieldValue{FieldType::FT_ARRAY, fieldValues});
    }
};