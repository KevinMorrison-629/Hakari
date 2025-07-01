#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

#include "core/persistence/CollectionEntry.h"
#include "core/persistence/QueryBuilder.h"

#include <bsoncxx/json.hpp>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/options/count.hpp>

namespace Core::Persistence
{
    template <typename T, typename std::enable_if_t<std::is_base_of<CollectionEntry, T>::value, int> = 0>
    class CollectionWrapper
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

        // Public Getter/Setter Functions

        std::vector<T> GetRandom(size_t count = 1U, const std::unordered_map<std::string, FieldValue> &fields = {})
        {
            mongocxx::pipeline pipeline{};

            // add a $match stage
            if (!fields.empty())
            {
                bsoncxx::builder::basic::document match_stage;
                for (const auto &[key, value] : fields)
                {
                    AppendToDocument(match_stage, key, value);
                }
                pipeline.match(match_stage.view());
            }

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

        std::vector<T> GetAll(Query &query, size_t max_count = 999)
        {
            bsoncxx::document::value filter = query.build();

            auto cursor = m_Collection.find(filter.view(), mongocxx::options::find{}.limit(max_count));

            std::vector<T> entries;
            for (auto &&doc : cursor)
            {
                T entry;
                entry.fromBson(doc);
                entries.push_back(std::move(entry));
            }
            return entries;
        }

        std::vector<T> GetAll(const std::unordered_map<std::string, FieldValue> &fields, size_t max_count = 999)
        {
            mongocxx::pipeline pipeline{};

            // If fields are provided, apply a $match filter
            if (!fields.empty())
            {
                bsoncxx::builder::basic::document match_stage;
                for (const auto &[key, value] : fields)
                {
                    AppendToDocument(match_stage, key, value);
                }
                pipeline.match(match_stage.view());
            }

            mongocxx::options::aggregate &options = mongocxx::options::aggregate();
            options.batch_size(max_count);
            auto cursor = m_Collection.aggregate(pipeline, options);

            std::vector<T> entries;
            for (auto &&doc : cursor)
            {
                T entry;
                entry.fromBson(doc);
                entries.push_back(std::move(entry));
            }

            return entries;
        }

        std::optional<bsoncxx::oid> Add(T entry)
        {
            try
            {
                // Convert entry object to BSON document
                bsoncxx::document::value doc = entry.toBson();
                // Assuming entry has a method toBson() that returns a BSON document

                // Insert into MongoDB collection
                auto result = m_Collection.insert_one(doc.view());

                // Check if insertion was successful and return the inserted _id.
                if (result && result->result().inserted_count() > 0)
                {
                    // The inserted_id is of type bsoncxx::types::value;
                    // get_oid() extracts the OID if the _id is of type OID.
                    return result->inserted_id().get_oid().value;
                }
                return std::nullopt;
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error inserting document: " << e.what() << std::endl;
                return std::nullopt;
            }
        }

        bool UpdateEntry(const std::unordered_map<std::string, FieldValue> &match,
                         const std::unordered_map<std::string, FieldValue> &fields)
        {
            try
            {
                if (match.empty() || fields.empty())
                {
                    std::cerr << "Error: Match and update fields cannot be empty." << std::endl;
                    return false;
                }

                // Build the filter document ($match criteria)
                bsoncxx::builder::basic::document filter_doc;
                for (const auto &[key, value] : match)
                {
                    AppendToDocument(filter_doc, key, value);
                }

                // Build the update document ($set fields)
                bsoncxx::builder::basic::document update_doc;
                bsoncxx::builder::basic::document set_doc;
                for (const auto &[key, value] : fields)
                {
                    AppendToDocument(set_doc, key, value);
                }
                update_doc.append(bsoncxx::builder::basic::kvp("$set", set_doc.view()));

                // Perform the update
                auto result = m_Collection.update_many(filter_doc.view(), update_doc.view());

                // Check if the update was successful
                if (result && result->modified_count() > 0)
                {
                    return true;
                }
                else
                {
                    std::cerr << "Warning: No documents were updated." << std::endl;
                    return false;
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error updating document: " << e.what() << std::endl;
                return false;
            }
        }

        bool DeleteEntry(const std::unordered_map<std::string, FieldValue> &match)
        {
            try
            {
                if (match.empty())
                {
                    std::cerr << "Error: Match fields cannot be empty." << std::endl;
                    return false;
                }

                // Build the filter document
                bsoncxx::builder::basic::document filter_doc;
                for (const auto &[key, value] : match)
                {
                    AppendToDocument(filter_doc, key, value);
                }

                // Perform the deletion
                auto result = m_Collection.delete_many(filter_doc.view());

                return result && result->deleted_count() > 0;
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error deleting document: " << e.what() << std::endl;
                return false;
            }
        }

        size_t CountEntries(const std::unordered_map<std::string, FieldValue> &match)
        {
            try
            {
                bsoncxx::builder::basic::document filter_doc;
                for (const auto &[key, value] : match)
                {
                    AppendToDocument(filter_doc, key, value);
                }

                return m_Collection.count_documents(filter_doc.view());
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error counting documents: " << e.what() << std::endl;
                return 0;
            }
        }

        std::vector<T> GetAllSorted(const std::string &sortField, bool ascending = true)
        {
            mongocxx::options::find opts;
            bsoncxx::builder::basic::document sort_doc;
            sort_doc.append(bsoncxx::builder::basic::kvp(sortField, ascending ? 1 : -1));

            opts.sort(sort_doc.view());

            auto cursor = m_Collection.find({}, opts);

            std::vector<T> entries;
            for (auto &&doc : cursor)
            {
                T entry;
                entry.fromBson(doc);
                entries.push_back(std::move(entry));
            }

            return entries;
        }

        bool PushToArray(const std::unordered_map<std::string, FieldValue> &match, const std::string &arrayField,
                         const FieldValue &newElement)
        {
            try
            {
                // Build filter document
                bsoncxx::builder::basic::document filter_doc;
                for (const auto &[key, value] : match)
                {
                    AppendToDocument(filter_doc, key, value);
                }

                // Build update document with $push operator
                bsoncxx::builder::basic::document update_doc;
                bsoncxx::builder::basic::document push_doc;
                AppendToDocument(push_doc, arrayField, newElement);
                update_doc.append(bsoncxx::builder::basic::kvp("$push", push_doc.view()));

                auto result = m_Collection.update_one(filter_doc.view(), update_doc.view());
                return result && result->modified_count() > 0;
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error pushing to array: " << e.what() << std::endl;
                return false;
            }
        }

        bool PullFromArray(const std::unordered_map<std::string, FieldValue> &match, const std::string &arrayField,
                           const FieldValue &valueToRemove)
        {
            try
            {
                // Build filter document
                bsoncxx::builder::basic::document filter_doc;
                for (const auto &[key, value] : match)
                {
                    AppendToDocument(filter_doc, key, value);
                }

                // Build update document with $pull operator
                bsoncxx::builder::basic::document update_doc;
                bsoncxx::builder::basic::document pull_doc;
                AppendToDocument(pull_doc, arrayField, valueToRemove);
                update_doc.append(bsoncxx::builder::basic::kvp("$pull", pull_doc.view()));

                auto result = m_Collection.update_one(filter_doc.view(), update_doc.view());
                return result && result->modified_count() > 0;
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error pulling from array: " << e.what() << std::endl;
                return false;
            }
        }

        bool DeleteArrayField(const std::unordered_map<std::string, FieldValue> &match, const std::string &arrayField)
        {
            try
            {
                // Build filter document
                bsoncxx::builder::basic::document filter_doc;
                for (const auto &[key, value] : match)
                {
                    AppendToDocument(filter_doc, key, value);
                }

                // Build update document with $unset operator
                bsoncxx::builder::basic::document update_doc;
                bsoncxx::builder::basic::document unset_doc;
                unset_doc.append(bsoncxx::builder::basic::kvp(arrayField, ""));
                update_doc.append(bsoncxx::builder::basic::kvp("$unset", unset_doc.view()));

                auto result = m_Collection.update_one(filter_doc.view(), update_doc.view());
                return result && result->modified_count() > 0;
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error unsetting array field: " << e.what() << std::endl;
                return false;
            }
        }

    private:
        mongocxx::v_noabi::collection m_Collection;
    };
}