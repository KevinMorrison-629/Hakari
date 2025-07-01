#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include <vector>
#include <iostream>

// Include MongoDB C++ driver headers for BSON building.
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/types.hpp>

#include "core/persistence/Field.h"

namespace Core::Persistence
{
    class CollectionEntry
    {
    public:
        virtual ~CollectionEntry() = default;

        // Check if the entry has a field.
        bool hasField(const std::string &fieldName) const { return fields.find(fieldName) != fields.end(); }

        // Retrieve a field’s value (or nullptr if missing).
        const FieldValue *getField(const std::string &fieldName) const
        {
            auto it = fields.find(fieldName);
            if (it != fields.end())
                return &(it->second);
            return nullptr;
        }

        // Set (or update) a field’s value.
        void setField(const std::string &fieldName, const FieldValue &fieldValue) { fields[fieldName] = fieldValue; }

        // Generic getter template.
        template <typename T>
        T getValue(const std::string &field, T defaultValue = T()) const
        {
            if (auto f = getField(field))
                return std::get<T>(f->value);
            return defaultValue;
        }

        // Generic setter template.
        template <typename T>
        void setValue(const std::string &field, const T &value)
        {
            setField(field, FieldValue{type_to_fieldtype<T>::value, value});
        }

        // For debugging: convert the document to a JSON-like string.
        std::string toJson() const { return bsoncxx::to_json(toBson()); }

        // Convert the in-memory document into a BSON document.
        bsoncxx::document::value toBson() const
        {
            bsoncxx::builder::basic::document doc{};
            for (const auto &pair : fields)
            {
                AppendToDocument(doc, pair.first, pair.second);
            }
            return doc.extract();
        }

        // Populate the in-memory document from a BSON document.
        void fromBson(const bsoncxx::document::view &doc)
        {
            fields.clear();
            for (auto element : doc)
            {
                std::string key{element.key()};
                FieldValue fv = fromBsonElement(element);
                fields[key] = fv;
            }
        }

    protected:
        // The underlying map of field names to their values.
        std::unordered_map<std::string, FieldValue> fields;

    private:
    };
}