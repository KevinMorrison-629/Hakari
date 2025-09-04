#pragma once

#include "quickdb/components/document.h"
#include "quickdb/quickdb.h"

#include <string>

namespace Core::Data
{

    /// Raw Data

    struct CharacterReference : public QDB::Document
    {
        bsoncxx::oid id;
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

            // fields["id"] = id;
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
            if (this->_id.type == QDB::FieldType::FT_OBJECT_ID)
            {
                this->id = this->_id.as<bsoncxx::oid>();
            }

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
        bsoncxx::oid id;
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
            if (this->_id.type == QDB::FieldType::FT_OBJECT_ID)
            {
                this->id = this->_id.as<bsoncxx::oid>();
            }

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
        bsoncxx::oid id;
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
            if (this->_id.type == QDB::FieldType::FT_OBJECT_ID)
            {
                this->id = this->_id.as<bsoncxx::oid>();
            }

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

    struct CardReference : public QDB::Document
    {
        bsoncxx::oid id;
        std::string uuid;

        std::string name;
        bsoncxx::oid characterId;
        bsoncxx::oid setId;
        int32_t tier;
        std::string image;
        int32_t numAcquired;

        virtual std::unordered_map<std::string, QDB::FieldValue> to_fields() const override
        {
            std::unordered_map<std::string, QDB::FieldValue> fields;

            fields["uuid"] = uuid;
            fields["name"] = name;
            fields["characterId"] = characterId;
            fields["setId"] = setId;
            fields["tier"] = tier;
            fields["image"] = image;
            fields["numAcquired"] = numAcquired;

            return fields;
        }

        virtual void from_fields(const std::unordered_map<std::string, QDB::FieldValue> &fields) override
        {
            QDB::get_field(fields, "_id", this->_id);
            if (this->_id.type == QDB::FieldType::FT_OBJECT_ID)
            {
                this->id = this->_id.as<bsoncxx::oid>();
            }

            QDB::get_field(fields, "uuid", uuid);
            QDB::get_field(fields, "name", name);
            QDB::get_field(fields, "characterId", characterId);
            QDB::get_field(fields, "setId", setId);
            QDB::get_field(fields, "tier", tier);
            QDB::get_field(fields, "image", image);
            QDB::get_field(fields, "numAcquired", numAcquired);
        }
    };

    struct CardObject : public QDB::Document
    {
        bsoncxx::oid id;
        std::string uuid;

        bsoncxx::oid cardReferenceId;
        int32_t number;
        int32_t quality;

        virtual std::unordered_map<std::string, QDB::FieldValue> to_fields() const override
        {
            std::unordered_map<std::string, QDB::FieldValue> fields;

            fields["uuid"] = uuid;
            fields["cardReferenceId"] = cardReferenceId;
            fields["number"] = number;
            fields["quality"] = quality;

            return fields;
        }

        virtual void from_fields(const std::unordered_map<std::string, QDB::FieldValue> &fields) override
        {
            QDB::get_field(fields, "_id", this->_id);
            if (this->_id.type == QDB::FieldType::FT_OBJECT_ID)
            {
                this->id = this->_id.as<bsoncxx::oid>();
            }

            QDB::get_field(fields, "uuid", uuid);
            QDB::get_field(fields, "cardReferenceId", cardReferenceId);
            QDB::get_field(fields, "number", number);
            QDB::get_field(fields, "quality", quality);
        }
    };

    struct Player : public QDB::Document
    {
        bsoncxx::oid id;
        int64_t discord_id;
        std::vector<bsoncxx::oid> inventory; // A list of CardObject IDs

        virtual std::unordered_map<std::string, QDB::FieldValue> to_fields() const override
        {
            std::unordered_map<std::string, QDB::FieldValue> fields;
            fields["discord_id"] = discord_id;
            fields["inventory"] = inventory;
            return fields;
        }

        virtual void from_fields(const std::unordered_map<std::string, QDB::FieldValue> &fields) override
        {
            QDB::get_field(fields, "_id", this->_id);
            if (this->_id.type == QDB::FieldType::FT_OBJECT_ID)
            {
                this->id = this->_id.as<bsoncxx::oid>();
            }

            QDB::get_field(fields, "discord_id", discord_id);
            QDB::get_field(fields, "inventory", inventory);
        }
    };

} // namespace Core::Data