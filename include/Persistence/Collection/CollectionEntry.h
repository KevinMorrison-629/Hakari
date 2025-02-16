#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <cstdint>

// Include MongoDB C++ driver headers for BSON building.
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/json.hpp>

#include "Persistence/Collection/FieldType.h"

// Forward declaration for self-referential variant.
struct FieldValue;

// Define a variant to hold different field types.
// (For example: a MongoDB ObjectId, numbers, booleans, strings, etc.)
using FieldVariant = std::variant<
    std::vector<FieldValue>,                    // For FT_ARRAY
    std::vector<uint8_t>,                       // For FT_BINARY
    bool,                                       // For FT_BOOLEAN
    int32_t,                                    // For FT_INT_32
    int64_t,                                    // For FT_INT_64
    double,                                     // For FT_DOUBLE
    std::nullptr_t,                             // For FT_NULL
    std::string,                                // For FT_STRING, FT_CODE, FT_OBJECT_ID, etc.
    bsoncxx::types::b_date,                     // FT_DATE
    bsoncxx::types::b_timestamp,                // FT_TIMESTAMP
    std::unordered_map<std::string, FieldValue> // For FT_OBJECT
    >;

// A field’s value includes its type and the actual value.
struct FieldValue
{
    FieldValue() = default;
    ~FieldValue() = default;
    FieldValue(const FieldType &_type, const FieldVariant &_val)
    {
        type = _type;
        value = _val;
    }

    FieldType type;
    FieldVariant value;
};

// Equality operator for FieldValue (for simple types).
inline bool operator==(const FieldValue &lhs, const FieldValue &rhs)
{
    if (lhs.type != rhs.type)
        return false;
    return lhs.value == rhs.value;
}

class CollectionEntry
{
public:
    virtual ~CollectionEntry() = default;

    // Check if the entry has a field.
    bool hasField(const std::string &fieldName) const
    {
        return fields.find(fieldName) != fields.end();
    }

    // Retrieve a field’s value (or nullptr if missing).
    const FieldValue *getField(const std::string &fieldName) const
    {
        auto it = fields.find(fieldName);
        if (it != fields.end())
            return &(it->second);
        return nullptr;
    }

    // Set (or update) a field’s value.
    void setField(const std::string &fieldName, const FieldValue &fieldValue)
    {
        fields[fieldName] = fieldValue;
    }

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
    void setValue(const std::string &field, FieldType type, const T &value)
    {
        setField(field, FieldValue{type, value});
    }

    // For debugging: convert the document to a JSON-like string.
    std::string toJson() const
    {
        return bsoncxx::to_json(toBson());
    }

    // Convert the in-memory document into a BSON document.
    bsoncxx::document::value toBson() const
    {
        using namespace bsoncxx::builder::stream;
        document builder{};
        for (const auto &pair : fields)
        {
            appendFieldValue(builder, pair.first, pair.second);
        }
        return builder << finalize;
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

    //---------------------------------------------------------------
    // Helper functions for converting FieldValue to BSON.
    //---------------------------------------------------------------

    // Append a FieldValue to a document builder given a key.
    static void appendFieldValue(bsoncxx::builder::stream::document &builder,
                                 const std::string &key,
                                 const FieldValue &fv)
    {
        using namespace bsoncxx::builder::stream;
        switch (fv.type)
        {
        case FieldType::FT_BOOLEAN:
            builder << key << std::get<bool>(fv.value);
            break;
        case FieldType::FT_INT_32:
            builder << key << std::get<int32_t>(fv.value);
            break;
        case FieldType::FT_INT_64:
            builder << key << std::get<int64_t>(fv.value);
            break;
        case FieldType::FT_DOUBLE:
            builder << key << std::get<double>(fv.value);
            break;
        case FieldType::FT_NULL:
            builder << key << bsoncxx::types::b_null{};
            break;
        case FieldType::FT_STRING:
            builder << key << std::get<std::string>(fv.value);
            break;
        case FieldType::FT_OBJECT_ID:
            builder << key << bsoncxx::oid{std::get<std::string>(fv.value)};
            break;
        case FieldType::FT_ARRAY:
        {
            auto arr = std::get<std::vector<FieldValue>>(fv.value);
            bsoncxx::builder::stream::array arr_builder{};
            for (const auto &item : arr)
            {
                appendFieldValue(arr_builder, item);
            }
            builder << key << arr_builder;
            break;
        }
        case FieldType::FT_DATE:
            builder << key << std::get<bsoncxx::types::b_date>(fv.value);
            break;
        case FieldType::FT_TIMESTAMP:
            builder << key << std::get<bsoncxx::types::b_timestamp>(fv.value);
            break;
        default:
            break;
        }
    }

    // Overload: Append a FieldValue to an array builder.
    static void appendFieldValue(bsoncxx::builder::stream::array &arr_builder,
                                 const FieldValue &fv)
    {
        using namespace bsoncxx::builder::stream;
        switch (fv.type)
        {
        case FieldType::FT_BOOLEAN:
            arr_builder << std::get<bool>(fv.value);
            break;
        case FieldType::FT_INT_32:
            arr_builder << std::get<int32_t>(fv.value);
            break;
        case FieldType::FT_INT_64:
            arr_builder << std::get<int64_t>(fv.value);
            break;
        case FieldType::FT_DOUBLE:
            arr_builder << std::get<double>(fv.value);
            break;
        case FieldType::FT_NULL:
            arr_builder << bsoncxx::types::b_null{};
            break;
        case FieldType::FT_STRING:
            arr_builder << std::get<std::string>(fv.value);
            break;
        case FieldType::FT_OBJECT_ID:
            arr_builder << bsoncxx::oid{std::get<std::string>(fv.value)};
            break;
        case FieldType::FT_ARRAY:
        {
            auto inner_arr = std::get<std::vector<FieldValue>>(fv.value);
            bsoncxx::builder::stream::array inner_arr_builder{};
            for (const auto &item : inner_arr)
            {
                appendFieldValue(inner_arr_builder, item);
            }
            arr_builder << inner_arr_builder;
            break;
        }
        case FieldType::FT_DATE:
            arr_builder << std::get<bsoncxx::types::b_date>(fv.value);
            break;
        case FieldType::FT_TIMESTAMP:
            arr_builder << std::get<bsoncxx::types::b_timestamp>(fv.value);
            break;
        default:
            break;
        }
    }

protected:
    // The underlying map of field names to their values.
    std::unordered_map<std::string, FieldValue> fields;

private:
    //---------------------------------------------------------------
    // Helper functions for converting BSON elements into FieldValue.
    //---------------------------------------------------------------

    // Convert a BSON document element to a FieldValue.
    static FieldValue fromBsonElement(const bsoncxx::document::element &element)
    {
        FieldValue fv;
        switch (element.type())
        {
        case bsoncxx::type::k_bool:
            fv.type = FieldType::FT_BOOLEAN;
            fv.value = element.get_bool();
            break;
        case bsoncxx::type::k_int32:
            fv.type = FieldType::FT_INT_32;
            fv.value = element.get_int32();
            break;
        case bsoncxx::type::k_int64:
            fv.type = FieldType::FT_INT_64;
            fv.value = element.get_int64();
            break;
        case bsoncxx::type::k_double:
            fv.type = FieldType::FT_DOUBLE;
            fv.value = element.get_double();
            break;
        case bsoncxx::type::k_null:
            fv.type = FieldType::FT_NULL;
            fv.value = nullptr;
            break;
        case bsoncxx::type::k_utf8:
            fv.type = FieldType::FT_STRING;
            fv.value = static_cast<std::string>(element.get_string().value);
            break;
        case bsoncxx::type::k_oid:
            fv.type = FieldType::FT_OBJECT_ID;
            fv.value = element.get_oid().value.to_string();
            break;
        case bsoncxx::type::k_array:
        {
            fv.type = FieldType::FT_ARRAY;
            std::vector<FieldValue> vec;
            auto arr_view = element.get_array().value;
            for (auto arr_elem : arr_view)
            {
                vec.push_back(fromBsonElement(arr_elem));
            }
            fv.value = vec;
            break;
        }
        case bsoncxx::type::k_date:
            fv.type = FieldType::FT_DATE;
            fv.value = element.get_date();
            break;
        case bsoncxx::type::k_timestamp:
            fv.type = FieldType::FT_TIMESTAMP;
            fv.value = element.get_timestamp();
            break;
        default:
            // For unsupported types, default to null.
            fv.type = FieldType::FT_NULL;
            fv.value = nullptr;
            break;
        }
        return fv;
    }

    // Overload for BSON array elements (they use the same interface).
    static FieldValue fromBsonElement(const bsoncxx::array::element &element)
    {
        FieldValue fv;
        switch (element.type())
        {
        case bsoncxx::type::k_bool:
            fv.type = FieldType::FT_BOOLEAN;
            fv.value = element.get_bool();
            break;
        case bsoncxx::type::k_int32:
            fv.type = FieldType::FT_INT_32;
            fv.value = element.get_int32();
            break;
        case bsoncxx::type::k_int64:
            fv.type = FieldType::FT_INT_64;
            fv.value = element.get_int64();
            break;
        case bsoncxx::type::k_double:
            fv.type = FieldType::FT_DOUBLE;
            fv.value = element.get_double();
            break;
        case bsoncxx::type::k_null:
            fv.type = FieldType::FT_NULL;
            fv.value = nullptr;
            break;
        case bsoncxx::type::k_utf8:
            fv.type = FieldType::FT_STRING;
            fv.value = static_cast<std::string>(element.get_string().value);
            break;
        case bsoncxx::type::k_oid:
            fv.type = FieldType::FT_OBJECT_ID;
            fv.value = element.get_oid().value.to_string();
            break;
        case bsoncxx::type::k_array:
        {
            fv.type = FieldType::FT_ARRAY;
            std::vector<FieldValue> vec;
            auto arr_view = element.get_array().value;
            for (auto arr_elem : arr_view)
            {
                vec.push_back(fromBsonElement(arr_elem));
            }
            fv.value = vec;
            break;
        }
        case bsoncxx::type::k_date:
            fv.type = FieldType::FT_DATE;
            fv.value = element.get_date();
            break;
        case bsoncxx::type::k_timestamp:
            fv.type = FieldType::FT_TIMESTAMP;
            fv.value = element.get_timestamp();
            break;
        default:
            fv.type = FieldType::FT_NULL;
            fv.value = nullptr;
            break;
        }
        return fv;
    }
};