#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

// Include MongoDB C++ driver headers for BSON building.
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/types.hpp>

namespace Core::Data
{
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

    template <typename T>
    struct type_to_fieldtype;

    template <>
    struct type_to_fieldtype<bool>
    {
        static constexpr FieldType value = FieldType::FT_BOOLEAN;
    };
    template <>
    struct type_to_fieldtype<int32_t>
    {
        static constexpr FieldType value = FieldType::FT_INT_32;
    };
    template <>
    struct type_to_fieldtype<int64_t>
    {
        static constexpr FieldType value = FieldType::FT_INT_64;
    };
    template <>
    struct type_to_fieldtype<double>
    {
        static constexpr FieldType value = FieldType::FT_DOUBLE;
    };
    template <>
    struct type_to_fieldtype<bsoncxx::oid>
    {
        static constexpr FieldType value = FieldType::FT_OBJECT_ID;
    };
    template <>
    struct type_to_fieldtype<std::string>
    {
        static constexpr FieldType value = FieldType::FT_STRING;
    };
    template <>
    struct type_to_fieldtype<bsoncxx::types::b_date>
    {
        static constexpr FieldType value = FieldType::FT_DATE;
    };
    template <>
    struct type_to_fieldtype<std::vector<FieldValue>>
    {
        static constexpr FieldType value = FieldType::FT_ARRAY;
    };
    template <>
    struct type_to_fieldtype<std::unordered_map<std::string, FieldValue>>
    {
        static constexpr FieldType value = FieldType::FT_OBJECT;
    };

    // Define a variant to hold different field types.
    // (For example: a MongoDB ObjectId, numbers, booleans, strings, etc.)
    using FieldVariant = std::variant<std::vector<FieldValue>,                    // For FT_ARRAY
                                      std::vector<uint8_t>,                       // For FT_BINARY
                                      bool,                                       // For FT_BOOLEAN
                                      int32_t,                                    // For FT_INT_32
                                      int64_t,                                    // For FT_INT_64
                                      double,                                     // For FT_DOUBLE
                                      std::nullptr_t,                             // For FT_NULL
                                      bsoncxx::oid,                               // <-- Add this for FT_OBJECT_ID
                                      std::string,                                // For FT_STRING, FT_CODE, etc.
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
    // Forward declarations for recursive calls
    static void AppendToDocument(bsoncxx::builder::basic::document &doc, const std::string &key, const FieldValue &fv);
    static void AppendToArray(bsoncxx::builder::basic::array &arr, const FieldValue &fv);
    template <typename BsonElement>
    static FieldValue fromBsonElement(const BsonElement &element);

    /**
     * @brief Appends a FieldValue to a BSON array builder.
     * @param arr The BSON array builder to append to.
     * @param fv The FieldValue to append.
     */
    static void AppendToArray(bsoncxx::builder::basic::array &arr, const FieldValue &fv)
    {
        using namespace bsoncxx::builder::basic;
        switch (fv.type)
        {
        case FieldType::FT_BOOLEAN:
        {
            arr.append(std::get<bool>(fv.value));
            break;
        }
        case FieldType::FT_INT_32:
        {
            arr.append(std::get<int32_t>(fv.value));
            break;
        }
        case FieldType::FT_INT_64:
        {
            arr.append(std::get<int64_t>(fv.value));
            break;
        }
        case FieldType::FT_DOUBLE:
        {
            arr.append(std::get<double>(fv.value));
            break;
        }
        case FieldType::FT_NULL:
        {
            arr.append(bsoncxx::types::b_null{});
            break;
        }
        case FieldType::FT_STRING:
        {
            arr.append(std::get<std::string>(fv.value));
            break;
        }
        case FieldType::FT_OBJECT_ID:
        {
            arr.append(std::get<bsoncxx::oid>(fv.value));
            break;
        }
        case FieldType::FT_DATE:
        {
            arr.append(std::get<bsoncxx::types::b_date>(fv.value));
            break;
        }
        case FieldType::FT_TIMESTAMP:
        {
            arr.append(std::get<bsoncxx::types::b_timestamp>(fv.value));
            break;
        }
        case FieldType::FT_OBJECT: // [NEW] Handles nested objects in an array
        {
            document sub_doc{};
            auto map = std::get<std::unordered_map<std::string, FieldValue>>(fv.value);
            for (const auto &[sub_key, sub_value] : map)
            {
                AppendToDocument(sub_doc, sub_key, sub_value); // Recursive call
            }
            arr.append(sub_doc.view());
            break;
        }
        case FieldType::FT_ARRAY: // [NEW] Handles nested arrays
        {
            array sub_arr{};
            auto vec = std::get<std::vector<FieldValue>>(fv.value);
            for (const auto &item : vec)
            {
                AppendToArray(sub_arr, item); // Recursive call
            }
            arr.append(sub_arr.view());
            break;
        }
        default:
            // Other types not handled, append null
            arr.append(bsoncxx::types::b_null{});
            break;
        }
    }

    /**
     * @brief Appends a key-FieldValue pair to a BSON document builder.
     * @param doc The BSON document builder to append to.
     * @param key The key for the new element.
     * @param fv The FieldValue to append.
     */
    static void AppendToDocument(bsoncxx::builder::basic::document &doc, const std::string &key, const FieldValue &fv)
    {
        using namespace bsoncxx::builder::basic;

        switch (fv.type)
        {
        case FieldType::FT_BOOLEAN:
        {
            doc.append(kvp(key, std::get<bool>(fv.value)));
            break;
        }
        case FieldType::FT_INT_32:
        {
            doc.append(kvp(key, std::get<int32_t>(fv.value)));
            break;
        }
        case FieldType::FT_INT_64:
        {
            doc.append(kvp(key, std::get<int64_t>(fv.value)));
            break;
        }
        case FieldType::FT_DOUBLE:
        {
            doc.append(kvp(key, std::get<double>(fv.value)));
            break;
        }
        case FieldType::FT_NULL:
        {
            doc.append(kvp(key, bsoncxx::types::b_null{}));
            break;
        }
        case FieldType::FT_STRING:
        {
            doc.append(kvp(key, std::get<std::string>(fv.value)));
            break;
        }
        case FieldType::FT_OBJECT_ID:
        {
            doc.append(kvp(key, std::get<bsoncxx::oid>(fv.value)));
            break;
        }
        case FieldType::FT_DATE:
        {
            doc.append(kvp(key, std::get<bsoncxx::types::b_date>(fv.value)));
            break;
        }
        case FieldType::FT_TIMESTAMP:
        {
            doc.append(kvp(key, std::get<bsoncxx::types::b_timestamp>(fv.value)));
            break;
        }
        case FieldType::FT_OBJECT: // [NEW] Handles nested objects
        {
            document sub_doc{};
            auto map = std::get<std::unordered_map<std::string, FieldValue>>(fv.value);
            for (const auto &[sub_key, sub_value] : map)
            {
                AppendToDocument(sub_doc, sub_key, sub_value); // Recursive call
            }
            doc.append(kvp(key, sub_doc.view()));
            break;
        }
        case FieldType::FT_ARRAY: // [NEW] Handles arrays
        {
            array sub_arr{};
            auto vec = std::get<std::vector<FieldValue>>(fv.value);
            for (const auto &item : vec)
            {
                AppendToArray(sub_arr, item); // Call array helper
            }
            doc.append(kvp(key, sub_arr.view()));
            break;
        }
        default:
        { // Other types not handled, append null
            doc.append(kvp(key, bsoncxx::types::b_null{}));
            break;
        }
        }
    }

    /**
     * @brief Converts any BSON element to a FieldValue.
     * @tparam BsonElement Can be bsoncxx::document::element or bsoncxx::array::element.
     * @param element The BSON element to convert.
     * @return The resulting FieldValue.
     */
    template <typename BsonElement>
    static FieldValue fromBsonElement(const BsonElement &element)
    {
        FieldValue fv;
        switch (element.type())
        {
        case bsoncxx::type::k_bool:
            fv.type = FieldType::FT_BOOLEAN;
            fv.value = element.get_bool().value;
            break;
        case bsoncxx::type::k_int32:
            fv.type = FieldType::FT_INT_32;
            fv.value = element.get_int32().value;
            break;
        case bsoncxx::type::k_int64:
            fv.type = FieldType::FT_INT_64;
            fv.value = element.get_int64().value;
            break;
        case bsoncxx::type::k_double:
            fv.type = FieldType::FT_DOUBLE;
            fv.value = element.get_double().value;
            break;
        case bsoncxx::type::k_string:
            fv.type = FieldType::FT_STRING;
            fv.value = static_cast<std::string>(element.get_string().value);
            break;
        case bsoncxx::type::k_oid:
            fv.type = FieldType::FT_OBJECT_ID;
            fv.value = element.get_oid().value;
            break;
        case bsoncxx::type::k_date:
            fv.type = FieldType::FT_DATE;
            fv.value = element.get_date();
            break;
        case bsoncxx::type::k_timestamp:
            fv.type = FieldType::FT_TIMESTAMP;
            fv.value = element.get_timestamp();
            break;
        case bsoncxx::type::k_document: // [NEW] Handles nested documents
        {
            fv.type = FieldType::FT_OBJECT;
            std::unordered_map<std::string, FieldValue> map;
            for (auto inner_element : element.get_document().value)
            {
                map[static_cast<std::string>(inner_element.key())] = fromBsonElement(inner_element); // Recursive call
            }
            fv.value = map;
            break;
        }
        case bsoncxx::type::k_array:
        {
            fv.type = FieldType::FT_ARRAY;
            std::vector<FieldValue> vec;
            for (auto inner_element : element.get_array().value)
            {
                vec.push_back(fromBsonElement(inner_element)); // Recursive call
            }
            fv.value = vec;
            break;
        }
        default:
            fv.type = FieldType::FT_NULL;
            fv.value = nullptr;
            break;
        }
        return fv;
    }
}