#pragma once

#include "quickdb/components/document.h"
#include "quickdb/quickdb.h"

#include <string>

namespace Core::Data
{
    enum Enum_ItemType : int8_t
    {
        DEFAULT,
        BORDER,
        NAME_TAG,
        HP_UPGRADE,
        AP_UPGRADE,
        INSURANCE_CARD,
    };

    enum Enum_CardTier : int8_t
    {
        CHAMPION,
        EXALTED,
        CELESTIAL,
        DIVINE,
        ASCENDANT,
        GENESIS,
        VOIDBORN,
        OMEGA,
    };

    inline std::string CardTier_to_String(const Enum_CardTier &tier)
    {
        switch (tier)
        {
        case (Enum_CardTier::CHAMPION):
        {
            return "champion";
        }
        case (Enum_CardTier::EXALTED):
        {
            return "exalted";
        }
        case (Enum_CardTier::CELESTIAL):
        {
            return "celestial";
        }
        case (Enum_CardTier::DIVINE):
        {
            return "divine";
        }
        case (Enum_CardTier::ASCENDANT):
        {
            return "ascendant";
        }
        case (Enum_CardTier::GENESIS):
        {
            return "genesis";
        }
        case (Enum_CardTier::VOIDBORN):
        {
            return "voidborn";
        }
        case (Enum_CardTier::OMEGA):
        {
            return "omega";
        }
        default:
        {
            return "default";
        }
        }
    }

    /// Raw Data

    struct CharacterReference : public QDB::Document
    {
        std::string uuid;
        int32_t mal_id;
        std::string name;
        std::string name_kanji;
        int32_t favorites;
        std::string about;
        std::string image;
        std::vector<std::string> nicknames;
        std::vector<bsoncxx::oid> anime_refs;
        std::vector<bsoncxx::oid> manga_refs;

        virtual std::unordered_map<std::string, QDB::FieldValue> to_fields() const override
        {
            std::unordered_map<std::string, QDB::FieldValue> fields;

            fields["uuid"] = uuid;
            fields["mal_id"] = mal_id;
            fields["name"] = name;
            fields["name_kanji"] = name_kanji;
            fields["favorites"] = favorites;
            fields["about"] = about;
            fields["image"] = image;
            fields["nicknames"] = nicknames;
            fields["anime_refs"] = anime_refs;
            fields["manga_refs"] = manga_refs;

            return fields;
        }

        virtual void from_fields(const std::unordered_map<std::string, QDB::FieldValue> &fields) override
        {
            // The new get_field helper simplifies deserialization.
            QDB::get_field(fields, "_id", this->_id);
            QDB::get_field(fields, "uuid", uuid);
            QDB::get_field(fields, "mal_id", mal_id);
            QDB::get_field(fields, "name", name);
            QDB::get_field(fields, "name_kanji", name_kanji);
            QDB::get_field(fields, "favorites", favorites);
            QDB::get_field(fields, "about", about);
            QDB::get_field(fields, "image", image);
            QDB::get_field(fields, "nicknames", nicknames);
            QDB::get_field(fields, "anime_refs", anime_refs);
            QDB::get_field(fields, "manga_refs", manga_refs);
        }
    };

    struct AnimeReference : public QDB::Document
    {
        std::string uuid;

        int32_t mal_id;
        std::string url;
        std::string title;
        int32_t episodes;
        std::string status;
        double score;
        std::string synopsis;

        virtual std::unordered_map<std::string, QDB::FieldValue> to_fields() const override
        {
            std::unordered_map<std::string, QDB::FieldValue> fields;

            fields["uuid"] = uuid;
            fields["mal_id"] = mal_id;
            fields["url"] = url;
            fields["title"] = title;
            fields["episodes"] = episodes;
            fields["status"] = status;
            fields["score"] = score;
            fields["synopsis"] = synopsis;

            return fields;
        }

        virtual void from_fields(const std::unordered_map<std::string, QDB::FieldValue> &fields) override
        {
            QDB::get_field(fields, "_id", this->_id);
            QDB::get_field(fields, "uuid", uuid);
            QDB::get_field(fields, "mal_id", mal_id);
            QDB::get_field(fields, "url", url);
            QDB::get_field(fields, "title", title);
            QDB::get_field(fields, "episodes", episodes);
            QDB::get_field(fields, "status", status);
            QDB::get_field(fields, "score", score);
            QDB::get_field(fields, "synopsis", synopsis);
        }
    };

    struct MangaReference : public QDB::Document
    {
        std::string uuid;

        int32_t mal_id;
        std::string url;
        std::string title;
        int32_t chapters;
        int32_t volumes;
        std::string status;
        double score;
        std::string synopsis;

        virtual std::unordered_map<std::string, QDB::FieldValue> to_fields() const override
        {
            std::unordered_map<std::string, QDB::FieldValue> fields;

            fields["uuid"] = uuid;
            fields["mal_id"] = mal_id;
            fields["url"] = url;
            fields["title"] = title;
            fields["chapters"] = chapters;
            fields["volumes"] = volumes;
            fields["status"] = status;
            fields["score"] = score;
            fields["synopsis"] = synopsis;

            return fields;
        }

        virtual void from_fields(const std::unordered_map<std::string, QDB::FieldValue> &fields) override
        {
            QDB::get_field(fields, "_id", this->_id);
            QDB::get_field(fields, "uuid", uuid);
            QDB::get_field(fields, "mal_id", mal_id);
            QDB::get_field(fields, "url", url);
            QDB::get_field(fields, "title", title);
            QDB::get_field(fields, "chapters", chapters);
            QDB::get_field(fields, "volumes", volumes);
            QDB::get_field(fields, "status", status);
            QDB::get_field(fields, "score", score);
            QDB::get_field(fields, "synopsis", synopsis);
        }
    };

    /// Game Data

    struct AbilityReference : public QDB::Document
    {
        std::string name;

        virtual std::unordered_map<std::string, QDB::FieldValue> to_fields() const override
        {
            std::unordered_map<std::string, QDB::FieldValue> fields;

            fields["name"] = name;
            return fields;
        }

        virtual void from_fields(const std::unordered_map<std::string, QDB::FieldValue> &fields) override
        {
            QDB::get_field(fields, "_id", this->_id);
            QDB::get_field(fields, "name", name);
        }
    };

    struct CardReference : public QDB::Document
    {
        std::string name;
        bsoncxx::oid characterId;
        bsoncxx::oid setId;
        Enum_CardTier tier;
        std::string image;
        bsoncxx::oid abilityId;

        int32_t numAcquired;
        int32_t lastSalePrice;

        virtual std::unordered_map<std::string, QDB::FieldValue> to_fields() const override
        {
            std::unordered_map<std::string, QDB::FieldValue> fields;

            fields["name"] = name;
            fields["characterId"] = characterId;
            fields["setId"] = setId;
            fields["tier"] = static_cast<int32_t>(tier);
            fields["image"] = image;
            fields["abilityId"] = abilityId;

            fields["numAcquired"] = numAcquired;
            fields["lastSalePrice"] = lastSalePrice;

            return fields;
        }

        virtual void from_fields(const std::unordered_map<std::string, QDB::FieldValue> &fields) override
        {
            int32_t temp_tier; // need to make a temp type to retrieve this enum from the database
            QDB::get_field(fields, "_id", this->_id);
            QDB::get_field(fields, "name", name);
            QDB::get_field(fields, "characterId", characterId);
            QDB::get_field(fields, "setId", setId);
            QDB::get_field(fields, "tier", temp_tier);
            QDB::get_field(fields, "image", image);
            QDB::get_field(fields, "abilityId", abilityId);

            QDB::get_field(fields, "numAcquired", numAcquired);
            QDB::get_field(fields, "lastSalePrice", lastSalePrice);

            tier = Enum_CardTier(temp_tier);
        }
    };

    struct ItemReference : public QDB::Document
    {
        std::string name;
        Enum_ItemType type;

        virtual std::unordered_map<std::string, QDB::FieldValue> to_fields() const override
        {
            std::unordered_map<std::string, QDB::FieldValue> fields;

            fields["name"] = name;
            fields["type"] = static_cast<int32_t>(type);
            return fields;
        }

        virtual void from_fields(const std::unordered_map<std::string, QDB::FieldValue> &fields) override
        {
            int32_t temp_type;
            QDB::get_field(fields, "_id", this->_id);
            QDB::get_field(fields, "name", name);
            QDB::get_field(fields, "type", temp_type);

            type = Enum_ItemType(temp_type);
        }
    };

    struct CardObject : public QDB::Document
    {
        bsoncxx::oid cardReferenceId;
        bsoncxx::oid ownerId;
        int32_t number;

        int32_t attackPoints;
        int32_t healthPoints;

        bsoncxx::oid customBorder;
        std::vector<bsoncxx::oid> ownerHistory;
        bsoncxx::types::b_timestamp lastAcquisitionDate;

        virtual std::unordered_map<std::string, QDB::FieldValue> to_fields() const override
        {
            std::unordered_map<std::string, QDB::FieldValue> fields;

            fields["cardReferenceId"] = cardReferenceId;
            fields["ownerId"] = ownerId;
            fields["number"] = number;

            fields["attackPoints"] = attackPoints;
            fields["healthPoints"] = healthPoints;

            fields["customBorder"] = customBorder;
            fields["ownerHistory"] = ownerHistory;
            fields["lastAcquisitionDate"] = lastAcquisitionDate;

            return fields;
        }

        virtual void from_fields(const std::unordered_map<std::string, QDB::FieldValue> &fields) override
        {
            QDB::get_field(fields, "_id", this->_id);
            QDB::get_field(fields, "cardReferenceId", cardReferenceId);
            QDB::get_field(fields, "ownerId", ownerId);
            QDB::get_field(fields, "number", number);

            QDB::get_field(fields, "attackPoints", attackPoints);
            QDB::get_field(fields, "healthPoints", healthPoints);

            QDB::get_field(fields, "customBorder", customBorder);
            QDB::get_field(fields, "ownerHistory", ownerHistory);
            QDB::get_field(fields, "lastAcquisitionDate", lastAcquisitionDate);
        }
    };

    struct Player : public QDB::Document
    {
        int64_t discord_id = 0;
        std::string display_name = "";
        std::string email = "";
        std::string password_hash = "";
        std::vector<bsoncxx::oid> items; // A list of ItemObject IDs
        std::vector<bsoncxx::oid> cards; // A list of CardObject IDs
        std::vector<std::vector<bsoncxx::oid>> decks = {std::vector<bsoncxx::oid>(), std::vector<bsoncxx::oid>(),
                                                        std::vector<bsoncxx::oid>()};

        int32_t pityScore = 0;
        int64_t essence = 0;
        bsoncxx::types::b_timestamp dailyBattleTimer;   // Player gets 100 essence on battle win (resets after 12 hr)
        bsoncxx::types::b_timestamp dailyFreePackTimer; // Player gets a free pack (resets after 12 hr)

        std::vector<bsoncxx::oid> friends = {};
        std::vector<bsoncxx::oid> friend_requests_sent = {};
        std::vector<bsoncxx::oid> friend_requests_received = {};

        virtual std::unordered_map<std::string, QDB::FieldValue> to_fields() const override
        {
            std::unordered_map<std::string, QDB::FieldValue> fields;
            fields["discord_id"] = discord_id;
            fields["displayName"] = display_name;
            fields["email"] = email;
            fields["password_hash"] = password_hash;
            fields["items"] = items;
            fields["cards"] = cards;
            fields["decks"] = decks;

            fields["pityScore"] = pityScore;
            fields["essence"] = essence;
            fields["dailyBattleTimer"] = dailyBattleTimer;
            fields["dailyFreePackTimer"] = dailyFreePackTimer;

            fields["friends"] = friends;
            fields["friendRequestsSent"] = friend_requests_sent;
            fields["friendRequestsReceived"] = friend_requests_received;

            return fields;
        }

        virtual void from_fields(const std::unordered_map<std::string, QDB::FieldValue> &fields) override
        {
            QDB::get_field(fields, "_id", this->_id);
            QDB::get_field(fields, "discord_id", discord_id);
            QDB::get_field(fields, "displayName", display_name);
            QDB::get_field(fields, "email", email);
            QDB::get_field(fields, "password_hash", password_hash);
            QDB::get_field(fields, "items", items);
            QDB::get_field(fields, "cards", cards);
            QDB::get_field(fields, "decks", decks);

            QDB::get_field(fields, "pityScore", pityScore);
            QDB::get_field(fields, "essence", essence);
            QDB::get_field(fields, "dailyBattleTimer", dailyBattleTimer);
            QDB::get_field(fields, "dailyFreePackTimer", dailyFreePackTimer);

            QDB::get_field(fields, "friends", friends);
            QDB::get_field(fields, "friendRequestsSent", friend_requests_sent);
            QDB::get_field(fields, "friendRequestsReceived", friend_requests_received);
        }
    };

} // namespace Core::Data