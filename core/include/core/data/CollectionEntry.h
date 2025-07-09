#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include <iostream>
#include <vector>

// Include MongoDB C++ driver headers for BSON building.
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/types.hpp>

#include "core/data/Field.h"

namespace Core::Data
{
    /// @brief Represents a generic entry in a database collection, essentially a document.
    /// This class provides a common interface for documents that can be stored in MongoDB,
    /// handling serialization to and from BSON, and offering generic field access.
    class CollectionEntry
    {
    public:
        /// @brief Virtual destructor to ensure proper cleanup of derived classes.
        virtual ~CollectionEntry() = default;

        /// @brief Checks if the entry contains a specific field.
        /// @param fieldName The name of the field to check for.
        /// @return True if the field exists, false otherwise.
        bool hasField(const std::string &fieldName) const { return fields.find(fieldName) != fields.end(); }

        /// @brief Retrieves a pointer to a field's value.
        /// @param fieldName The name of the field to retrieve.
        /// @return A const pointer to the FieldValue if found, otherwise nullptr.
        const FieldValue *getField(const std::string &fieldName) const
        {
            auto it = fields.find(fieldName);
            if (it != fields.end())
                return &(it->second);
            return nullptr;
        }

        /// @brief Sets or updates a field's value.
        /// @param fieldName The name of the field to set or update.
        /// @param fieldValue The FieldValue to assign to the field.
        void setField(const std::string &fieldName, const FieldValue &fieldValue) { fields[fieldName] = fieldValue; }

        /// @brief Generic template getter for retrieving a field's value with a specific type.
        /// @tparam T The expected C++ type of the field's value.
        /// @param field The name of the field to retrieve.
        /// @param defaultValue The value to return if the field is not found or type mismatch occurs.
        /// @return The field's value cast to type T, or defaultValue if not found/mismatched.
        template <typename T> T getValue(const std::string &field, T defaultValue = T()) const
        {
            if (const FieldValue *f = getField(field))
            {
                // Check if the variant holds the requested type T
                if (std::holds_alternative<T>(f->value))
                {
                    return std::get<T>(f->value);
                }
                // TODO: Consider logging a warning or throwing an error on type mismatch
                // if strict type checking is desired over returning defaultValue.
            }
            return defaultValue;
        }

        /// @brief Generic template setter for setting a field's value.
        /// Automatically determines the FieldType based on the C++ type T.
        /// @tparam T The C++ type of the value being set.
        /// @param field The name of the field to set.
        /// @param value The value to set for the field.
        template <typename T> void setValue(const std::string &field, const T &value)
        {
            setField(field, FieldValue{type_to_fieldtype<T>::value, value});
        }

        /// @brief Converts the collection entry (document) to a JSON string for debugging.
        /// @return A JSON string representation of the entry.
        std::string toJson() const { return bsoncxx::to_json(toBson()); }

        /// @brief Converts the in-memory representation of the entry into a BSON document.
        /// Iterates over all fields and uses AppendToDocument to build the BSON structure.
        /// @return A bsoncxx::document::value representing the entry.
        bsoncxx::document::value toBson() const
        {
            bsoncxx::builder::basic::document doc{};
            for (const auto &pair : fields)
            {
                AppendToDocument(doc, pair.first, pair.second);
            }
            return doc.extract();
        }

        /// @brief Populates the in-memory entry from a BSON document view.
        /// Clears existing fields and then iterates over the BSON document's elements,
        /// converting them to FieldValue objects and storing them.
        /// @param doc The bsoncxx::document::view to populate from.
        void fromBson(const bsoncxx::document::view &doc)
        {
            fields.clear();
            for (auto element : doc)
            {
                std::string key{element.key().data(), element.key().length()}; // Ensure correct string construction
                FieldValue fv = fromBsonElement(element);                      // fromBsonElement is defined in Field.h
                fields[key] = fv;
            }
        }

    protected:
        /// @brief The underlying storage for the entry's fields.
        /// An unordered map where keys are field names (strings) and values are FieldValue objects.
        std::unordered_map<std::string, FieldValue> fields;

    private:
        // No private members in this version.
    };
} // namespace Core::Data