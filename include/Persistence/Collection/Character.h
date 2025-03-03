#pragma once

#include "Persistence/CollectionEntry.h"
#include <string>

class Character : public CollectionEntry
{
public:
    Character() = default;

    // Constructor that sets all fields using our generic setters.
    Character(const std::string &_id, const std::string &_uuid, const std::string &_name, const std::string &_anime,
              const std::string &_description, const std::vector<std::string> &_images, const int64_t &_numWishes,
              const int64_t &_rank)
    {
        setId(_id);
        setUuid(_uuid);
        setName(_name);
        setAnime(_anime);
        setDescription(_description);
        setImages(_images);
        setNumWishes(_numWishes);
        setRank(_rank);
    }

    // Specific Getters
    std::string getId() const { return getValue<std::string>("_id"); }
    std::string getUuid() const { return getValue<std::string>("uuid"); }
    std::string getName() const { return getValue<std::string>("name"); }
    std::string getAnime() const { return getValue<std::string>("anime"); }
    std::string getDescription() const { return getValue<std::string>("description"); }
    int64_t getNumWishes() const { return getValue<int64_t>("num_wishes"); }
    int64_t getRank() const { return getValue<int64_t>("rank"); }

    std::vector<std::string> getImages() const
    {
        std::vector<std::string> result;
        if (auto f = getField("images"))
        {
            auto &arrayValues = std::get<std::vector<FieldValue>>(f->value);
            for (const auto &val : arrayValues)
            {
                result.push_back(std::get<std::string>(val.value));
            }
        }
        return result;
    }

    // Specific Setters
    void setId(const std::string &id) { setValue<std::string>("_id", FieldType::FT_OBJECT_ID, id); }
    void setUuid(const std::string &uuid) { setValue<std::string>("uuid", FieldType::FT_STRING, uuid); }
    void setName(const std::string &name) { setValue<std::string>("name", FieldType::FT_STRING, name); }
    void setAnime(const std::string &anime) { setValue<std::string>("anime", FieldType::FT_STRING, anime); }
    void setDescription(const std::string &desc) { setValue<std::string>("description", FieldType::FT_STRING, desc); }
    void setNumWishes(const int64_t &nwish) { setValue<int64_t>("num_wishes", FieldType::FT_INT_64, nwish); }
    void setRank(const int64_t &rank) { setValue<int64_t>("rank", FieldType::FT_INT_64, rank); }

    void setImages(const std::vector<std::string> &imageList)
    {
        std::vector<FieldValue> imageFieldValues;
        for (const auto &image : imageList)
        {
            imageFieldValues.push_back(FieldValue{FieldType::FT_STRING, image});
        }
        setField("images", FieldValue{FieldType::FT_ARRAY, imageFieldValues});
    }
};