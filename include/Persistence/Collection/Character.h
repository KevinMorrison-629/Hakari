#pragma once

#include "Persistence/Collection/CollectionEntry.h"
#include <string>

class Character : public CollectionEntry
{
public:
    Character() = default;

    // Constructor that sets all fields using our generic setters.
    Character(const std::string &_id, const std::string &_name, const std::string &_anime, const std::string &_description,
              const std::string &_relPath, const std::vector<std::string> &_images)
    {
        setId(_id);
        setName(_name);
        setAnime(_anime);
        setDescription(_description);
        setRelImgPath(_relPath);
        setImages(_images);
    }

    // Specific Getters
    std::string getId() const { return getValue<std::string>("_id"); }
    std::string getName() const { return getValue<std::string>("name"); }
    std::string getAnime() const { return getValue<std::string>("anime"); }
    std::string getRelImgPath() const { return getValue<std::string>("relative_path"); }
    std::string getDescription() const { return getValue<std::string>("description"); }

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
    void setName(const std::string &name) { setValue<std::string>("name", FieldType::FT_STRING, name); }
    void setAnime(const std::string &anime) { setValue<std::string>("anime", FieldType::FT_STRING, anime); }
    void setDescription(const std::string &desc) { setValue<std::string>("description", FieldType::FT_STRING, desc); }
    void setRelImgPath(const std::string &relpath) { setValue<std::string>("relative_path", FieldType::FT_STRING, relpath); }

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