#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

// Include MongoDB C++ driver headers for BSON building.
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/types.hpp>

enum class FieldType : uint8_t
{
    FT_ARRAY,
    FT_BINARY,
    FT_BOOLEAN,
    FT_CODE,
    FT_DATE,
    FT_DECIMAL_128,
    FT_DOUBLE,
    FT_INT_32,
    FT_INT_64,
    FT_MAXKEY,
    FT_MINKEY,
    FT_NULL,
    FT_OBJECT,
    FT_OBJECT_ID,
    FT_BSON_REG_EXPR,
    FT_STRING,
    FT_BSON_SYMBOL,
    FT_TIMESTAMP,
    FT_UNDEFINED,
};

// Forward declaration for self-referential variant.
struct FieldValue;

// Define a variant to hold different field types.
// (For example: a MongoDB ObjectId, numbers, booleans, strings, etc.)
using FieldVariant = std::variant<std::vector<FieldValue>,                    // For FT_ARRAY
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

//---------------------------------------------------------------
// Helper functions for converting FieldValue to BSON.
//---------------------------------------------------------------

static bsoncxx::builder::basic::array MakeDocumentArray(const FieldValue &fv)
{
    bsoncxx::builder::basic::array array;

    using namespace bsoncxx::builder::stream;
    switch (fv.type)
    {
    case FieldType::FT_BOOLEAN:
        array.append(std::get<bool>(fv.value));
        break;
    case FieldType::FT_INT_32:
        array.append(std::get<int32_t>(fv.value));
        break;
    case FieldType::FT_INT_64:
        array.append(std::get<int64_t>(fv.value));
        break;
    case FieldType::FT_DOUBLE:
        array.append(std::get<double>(fv.value));
        break;
    case FieldType::FT_NULL:
        array.append(bsoncxx::types::b_null{});
        break;
    case FieldType::FT_STRING:
        array.append(std::get<std::string>(fv.value));
        break;
    case FieldType::FT_OBJECT_ID:
        array.append(bsoncxx::oid{std::get<std::string>(fv.value)});
        break;
    case FieldType::FT_ARRAY:
    {
        // Build the nested BSON array.
        auto inner_arr = std::get<std::vector<FieldValue>>(fv.value);
        bsoncxx::builder::basic::array inner_array{};
        for (const auto &item : inner_arr)
        {
            inner_array.append(MakeDocumentArray(item));
        }
        // Append the inner array view.
        array.append(inner_array);
        break;
    }
    case FieldType::FT_DATE:
        array.append(std::get<bsoncxx::types::b_date>(fv.value));
        break;
    case FieldType::FT_TIMESTAMP:
        array.append(std::get<bsoncxx::types::b_timestamp>(fv.value));
        break;
    default:
        break;
    }

    return array;
}

static bsoncxx::builder::concatenate_doc MakeDocument(const std::string &key, const FieldValue &fv)
{
    bsoncxx::builder::basic::document doc{};

    using namespace bsoncxx::builder::stream;
    switch (fv.type)
    {
    case FieldType::FT_BOOLEAN:
        doc.append(bsoncxx::builder::basic::kvp(key, std::get<bool>(fv.value)));
        break;
    case FieldType::FT_INT_32:
        doc.append(bsoncxx::builder::basic::kvp(key, std::get<int32_t>(fv.value)));
        break;
    case FieldType::FT_INT_64:
        doc.append(bsoncxx::builder::basic::kvp(key, std::get<int64_t>(fv.value)));
        break;
    case FieldType::FT_DOUBLE:
        doc.append(bsoncxx::builder::basic::kvp(key, std::get<double>(fv.value)));
        break;
    case FieldType::FT_NULL:
        doc.append(bsoncxx::builder::basic::kvp(key, bsoncxx::types::b_null{}));
        break;
    case FieldType::FT_STRING:
        doc.append(bsoncxx::builder::basic::kvp(key, std::get<std::string>(fv.value)));
        break;
    case FieldType::FT_OBJECT_ID:
        doc.append(bsoncxx::builder::basic::kvp(key, bsoncxx::oid{std::get<std::string>(fv.value)}));
        break;
    case FieldType::FT_ARRAY:
    {
        // Build the BSON array from the vector<FieldValue>
        auto arr = std::get<std::vector<FieldValue>>(fv.value);
        bsoncxx::builder::basic::array arr_builder{};
        for (const auto &item : arr)
        {
            arr_builder.append(MakeDocumentArray(item));
        }
        // Append the array view to the document.
        doc.append(bsoncxx::builder::basic::kvp(key, arr_builder));
        break;
    }
    case FieldType::FT_DATE:
        doc.append(bsoncxx::builder::basic::kvp(key, std::get<bsoncxx::types::b_date>(fv.value)));
        break;
    case FieldType::FT_TIMESTAMP:
        doc.append(bsoncxx::builder::basic::kvp(key, std::get<bsoncxx::types::b_timestamp>(fv.value)));
        break;
    default:
        break;
    }

    return bsoncxx::builder::basic::concatenate(doc.extract());
}

//---------------------------------------------------------------
// Helper functions for converting BSON elements into FieldValue.
//---------------------------------------------------------------

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
