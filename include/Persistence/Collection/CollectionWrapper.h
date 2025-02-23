#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Persistence/Collection/CollectionEntry.h"

#include <bsoncxx/json.hpp>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

template <typename T, typename std::enable_if_t<std::is_base_of<CollectionEntry, T>::value, int> = 0> class CollectionWrapper
{
public:
    CollectionWrapper() = default;
    ~CollectionWrapper() = default;

    // Constructor accepting connection details.
    CollectionWrapper(const mongocxx::v_noabi::collection &collection) { m_Collection = collection; }

    // Insert an entry into the simulated collection.
    void insert(const T &entry)
    {
        auto bson_doc = entry.toBson();
        auto result = m_Collection.insert_one(bson_doc.view());
    }

    // (Optional) Query functions can also use the MongoDB C++ driver.
    // For example:
    std::vector<T> findByField(const std::string &fieldName, const FieldValue &value)
    {
        std::vector<T> results;
        // Build a query document using the BSON stream builder.
        using namespace bsoncxx::builder::stream;
        document query_builder;
        switch (value.type)
        {
        case FieldType::FT_BOOLEAN:
            query_builder << fieldName << std::get<bool>(value.value);
            break;
        case FieldType::FT_INT_32:
            query_builder << fieldName << std::get<int32_t>(value.value);
            break;
        case FieldType::FT_INT_64:
            query_builder << fieldName << std::get<int64_t>(value.value);
            break;
        case FieldType::FT_DOUBLE:
            query_builder << fieldName << std::get<double>(value.value);
            break;
        case FieldType::FT_NULL:
            query_builder << fieldName << bsoncxx::types::b_null{};
            break;
        case FieldType::FT_STRING:
        case FieldType::FT_CODE:
            query_builder << fieldName << std::get<std::string>(value.value);
            break;
        case FieldType::FT_OBJECT_ID:
        {
            std::string oidStr = std::get<std::string>(value.value);
            query_builder << fieldName << bsoncxx::oid{oidStr};
        }
        break;
        default:
            break;
        }
        auto query = query_builder << bsoncxx::builder::stream::finalize;
        auto cursor = m_Collection.find(query.view());
        for (auto &&doc : cursor)
        {
            T entry;
            entry.fromBson(doc); // Assumes EntryType provides a fromBson() method.
            results.push_back(entry);
        }
        return results;
    }

    std::vector<T> getRandom(size_t count)
    {
        mongocxx::pipeline pipeline{};
        pipeline.sample(static_cast<int32_t>(count));
        auto cursor = m_Collection.aggregate(pipeline, mongocxx::options::aggregate{});

        std::vector<T> entries;
        for (auto &&doc : cursor)
        {
            T entry;
            entry.fromBson(doc);
            entries.push_back(std::move(entry));
        }
        return entries;
    }

    void updateEntryById(const std::string &id, const std::unordered_map<std::string, FieldValue> &fields)
    {
        // Build the filter: { "_id": ObjectId(id) }
        bsoncxx::builder::stream::document filterBuilder{};
        filterBuilder << "_id" << bsoncxx::oid{id} << bsoncxx::builder::stream::finalize;

        // Build the update document.
        // We'll create a "$set" update that only updates the fields provided.
        bsoncxx::builder::stream::document setBuilder{};
        for (const auto &updatePair : fields)
        {
            // Append the update for each field.
            // This uses our helper function defined in CollectionEntry.
            CollectionEntry::appendFieldValue(setBuilder, updatePair.first, updatePair.second);
        }

        bsoncxx::builder::stream::document updateBuilder{};
        updateBuilder << "$set" << setBuilder.view() << bsoncxx::builder::stream::finalize;

        // Perform the update operation.
        auto result = m_Collection.update_one(filterBuilder.view(), updateBuilder.view());
    }

private:
    mongocxx::v_noabi::collection m_Collection;
};