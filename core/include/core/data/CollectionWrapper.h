#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/data/CollectionEntry.h"

#include <bsoncxx/json.hpp>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/count.hpp>
#include <mongocxx/uri.hpp>

namespace Core::Data
{
    /// @brief A template class that wraps a `mongocxx::collection` to provide
    /// type-safe operations for a specific document type `T`.
    /// @tparam T The type of the collection entry, which must derive from `CollectionEntry`.
    /// The SFINAE `std::enable_if_t` ensures that `T` is a subclass of `CollectionEntry`.
    template <typename T, typename std::enable_if_t<std::is_base_of<CollectionEntry, T>::value, int> = 0>
    class CollectionWrapper
    {
    public:
        /// @brief Default constructor.
        CollectionWrapper() = default;
        /// @brief Default destructor.
        ~CollectionWrapper() = default;

        /// @brief Constructor that initializes the wrapper with a MongoDB collection object.
        /// @param collection A const reference to a `mongocxx::collection` object.
        CollectionWrapper(const mongocxx::v_noabi::collection &collection) : m_Collection(collection) {}

        /// @brief Inserts a single entry into the collection.
        /// The entry is first converted to its BSON representation.
        /// @param entry The entry of type T to insert.
        void insert(const T &entry)
        {
            auto bson_doc = entry.toBson();
            m_Collection.insert_one(bson_doc.view());
            // Note: Does not check or return the result of insert_one.
        }

        /// @brief Retrieves a specified number of random entries from the collection, optionally matching given fields.
        /// Uses MongoDB's $sample aggregation pipeline stage.
        /// @param count The number of random entries to retrieve. Defaults to 1.
        /// @param fields An optional map of field names to FieldValue objects to filter entries before sampling.
        /// @return A std::vector of T entries. Returns an empty vector if no matching documents are found or on error.
        std::vector<T> GetRandom(size_t count = 1U, const std::unordered_map<std::string, FieldValue> &fields = {})
        {
            mongocxx::pipeline pipeline{};

            // Add a $match stage if fields are provided for filtering
            if (!fields.empty())
            {
                bsoncxx::builder::basic::document match_stage;
                for (const auto &[key, value] : fields)
                {
                    AppendToDocument(match_stage, key, value); // AppendToDocument is from Field.h
                }
                pipeline.match(match_stage.view());
            }

            pipeline.sample(static_cast<int32_t>(count));
            auto cursor = m_Collection.aggregate(pipeline, mongocxx::options::aggregate{});

            std::vector<T> entries;
            entries.reserve(count); // Pre-allocate for expected number of entries
            for (auto &&doc : cursor)
            {
                T entry;
                entry.fromBson(doc);
                entries.push_back(std::move(entry));
            }
            return entries;
        }

        /// @brief Retrieves all entries from the collection that match the given fields, up to a maximum count.
        /// @param fields A map of field names to FieldValue objects to filter entries. If empty, matches all documents.
        /// @param max_count The maximum number of entries to retrieve. Defaults to 999.
        /// @return A std::vector of T entries.
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
            // Add a $limit stage to cap the number of results if max_count is less than default or a very large number.
            if (max_count > 0)
            { // Ensure max_count is positive
                pipeline.limit(static_cast<int64_t>(max_count));
            }

            mongocxx::options::aggregate options = mongocxx::options::aggregate();
            // options.batch_size(max_count); // This controls cursor batching, not total document limit.
            // If max_count is truly a limit, it should be in the pipeline.

            auto cursor = m_Collection.aggregate(pipeline, options);

            std::vector<T> entries;
            entries.reserve(max_count);
            for (auto &&doc : cursor)
            {
                T entry;
                entry.fromBson(doc);
                entries.push_back(std::move(entry));
            }

            return entries;
        }

        /// @brief Adds a new entry to the collection and returns its generated ObjectId.
        /// @param entry The entry of type T to add.
        /// @return An std::optional<bsoncxx::oid> containing the ObjectId of the inserted document if successful,
        /// std::nullopt otherwise or on error.
        std::optional<bsoncxx::oid> Add(T entry) // Pass by value to allow potential move if T supports it
        {
            try
            {
                bsoncxx::document::value doc = entry.toBson();
                auto result = m_Collection.insert_one(doc.view());

                if (result && result->result().inserted_count() > 0)
                {
                    // Ensure the inserted_id is indeed an OID before trying to get it.
                    if (result->inserted_id().type() == bsoncxx::type::k_oid)
                    {
                        return result->inserted_id().get_oid().value;
                    }
                }
                return std::nullopt;
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error inserting document: " << e.what() << std::endl;
                return std::nullopt;
            }
        }

        /// @brief Updates existing entries in the collection that match the given criteria.
        /// Uses $set to update fields.
        /// @param match A map of field names to FieldValue objects specifying the criteria for documents to update.
        /// @param fields A map of field names to FieldValue objects specifying the fields to update and their new values.
        /// @return True if at least one document was modified, false otherwise or on error.
        bool UpdateEntry(const std::unordered_map<std::string, FieldValue> &match,
                         const std::unordered_map<std::string, FieldValue> &fields)
        {
            try
            {
                if (match.empty()) // Fields can be empty if only $unset or other ops are used, but for $set, it implies no
                                   // change.
                {
                    std::cerr << "Error: Match criteria cannot be empty for UpdateEntry." << std::endl;
                    return false;
                }
                if (fields.empty())
                {
                    std::cerr << "Warning: Update fields are empty, no update will be performed." << std::endl;
                    return false; // Or true, if no error but no change is considered success.
                }

                bsoncxx::builder::basic::document filter_doc;
                for (const auto &[key, value] : match)
                {
                    AppendToDocument(filter_doc, key, value);
                }

                bsoncxx::builder::basic::document update_payload_doc;
                for (const auto &[key, value] : fields)
                {
                    AppendToDocument(update_payload_doc, key, value);
                }
                bsoncxx::builder::basic::document update_doc;
                update_doc.append(bsoncxx::builder::basic::kvp("$set", update_payload_doc.view()));

                auto result = m_Collection.update_many(filter_doc.view(), update_doc.view());

                if (result)
                { // Result object exists
                    // Consider result->matched_count() if you want to know if anything matched.
                    return result->modified_count() > 0;
                }
                return false; // Should not happen if no exception, but defensive.
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error updating document(s): " << e.what() << std::endl;
                return false;
            }
        }

        /// @brief Deletes entries from the collection that match the given criteria.
        /// @param match A map of field names to FieldValue objects specifying the criteria for documents to delete.
        /// @return True if at least one document was deleted, false otherwise or on error.
        bool DeleteEntry(const std::unordered_map<std::string, FieldValue> &match)
        {
            try
            {
                if (match.empty())
                {
                    std::cerr << "Error: Match criteria cannot be empty for DeleteEntry. To delete all, use a specific "
                                 "method or pass an empty document explicitly."
                              << std::endl;
                    return false;
                }

                bsoncxx::builder::basic::document filter_doc;
                for (const auto &[key, value] : match)
                {
                    AppendToDocument(filter_doc, key, value);
                }

                auto result = m_Collection.delete_many(filter_doc.view());

                return result && result->deleted_count() > 0;
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error deleting document(s): " << e.what() << std::endl;
                return false;
            }
        }

        /// @brief Counts the number of entries in the collection that match the given criteria.
        /// @param match A map of field names to FieldValue objects specifying the criteria. If empty, counts all documents.
        /// @return The number of matching documents, or 0 on error.
        size_t
        CountEntries(const std::unordered_map<std::string, FieldValue> &match = {}) // Default to empty map for counting all
        {
            try
            {
                bsoncxx::builder::basic::document filter_doc;
                if (!match.empty())
                {
                    for (const auto &[key, value] : match)
                    {
                        AppendToDocument(filter_doc, key, value);
                    }
                }
                // If match is empty, filter_doc remains empty, which counts all documents.
                return m_Collection.count_documents(match.empty() ? bsoncxx::document::view{} : filter_doc.view());
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error counting documents: " << e.what() << std::endl;
                return 0;
            }
        }

        /// @brief Retrieves all entries from the collection, sorted by a specified field.
        /// @param sortField The name of the field to sort by.
        /// @param ascending If true, sorts in ascending order; otherwise, sorts in descending order. Defaults to true.
        /// @return A std::vector of T entries, sorted as specified.
        std::vector<T> GetAllSorted(const std::string &sortField, bool ascending = true)
        {
            mongocxx::options::find opts;
            bsoncxx::builder::basic::document sort_doc;
            sort_doc.append(bsoncxx::builder::basic::kvp(sortField, ascending ? 1 : -1));

            opts.sort(sort_doc.view());

            auto cursor = m_Collection.find({}, opts); // Empty filter means all documents

            std::vector<T> entries;
            for (auto &&doc : cursor)
            {
                T entry;
                entry.fromBson(doc);
                entries.push_back(std::move(entry));
            }

            return entries;
        }

        /// @brief Pushes a new element onto an array field within a document matching the criteria.
        /// Uses MongoDB's $push operator. Affects the first document matched.
        /// @param match A map defining the criteria to find the target document.
        /// @param arrayField The name of the array field to modify.
        /// @param newElement The FieldValue representing the element to add to the array.
        /// @return True if the update was successful and one document was modified, false otherwise or on error.
        bool PushToArray(const std::unordered_map<std::string, FieldValue> &match, const std::string &arrayField,
                         const FieldValue &newElement)
        {
            try
            {
                if (match.empty())
                {
                    std::cerr << "Error: Match criteria cannot be empty for PushToArray." << std::endl;
                    return false;
                }

                bsoncxx::builder::basic::document filter_doc;
                for (const auto &[key, value] : match)
                {
                    AppendToDocument(filter_doc, key, value);
                }

                bsoncxx::builder::basic::document push_payload_doc;
                // The newElement itself might be complex (e.g. a document), AppendToDocument handles this.
                // However, $push expects { arrayField: valueToPush } or { arrayField: { $each: [valuesToPush] } }
                // Here, we are pushing a single FieldValue, which could be a simple type or a document/array itself.
                // The BSON structure for $push { field: value } is what AppendToDocument creates if field is arrayField and
                // value is newElement. Let's ensure the value part is correctly BSON-ified for the $push operation. This is
                // tricky because AppendToDocument expects a key. We need to build the { arrayField: BSON_VALUE_OF_newElement
                // } for the $push operator. A direct AppendToDocument(push_payload_doc, arrayField, newElement) is correct
                // here.
                AppendToDocument(push_payload_doc, arrayField, newElement);

                bsoncxx::builder::basic::document update_doc;
                update_doc.append(bsoncxx::builder::basic::kvp("$push", push_payload_doc.view()));

                auto result = m_Collection.update_one(filter_doc.view(), update_doc.view());
                return result && result->modified_count() > 0;
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error pushing to array: " << e.what() << std::endl;
                return false;
            }
        }

        /// @brief Removes all instances of a specific value from an array field within a document matching the criteria.
        /// Uses MongoDB's $pull operator. Affects the first document matched.
        /// @param match A map defining the criteria to find the target document.
        /// @param arrayField The name of the array field to modify.
        /// @param valueToRemove The FieldValue representing the element to remove from the array.
        /// @return True if the update was successful and one document was modified, false otherwise or on error.
        bool PullFromArray(const std::unordered_map<std::string, FieldValue> &match, const std::string &arrayField,
                           const FieldValue &valueToRemove)
        {
            try
            {
                if (match.empty())
                {
                    std::cerr << "Error: Match criteria cannot be empty for PullFromArray." << std::endl;
                    return false;
                }
                bsoncxx::builder::basic::document filter_doc;
                for (const auto &[key, value] : match)
                {
                    AppendToDocument(filter_doc, key, value);
                }

                bsoncxx::builder::basic::document pull_payload_doc;
                // Similar to $push, $pull expects { arrayField: value_to_remove_or_condition }
                AppendToDocument(pull_payload_doc, arrayField, valueToRemove);

                bsoncxx::builder::basic::document update_doc;
                update_doc.append(bsoncxx::builder::basic::kvp("$pull", pull_payload_doc.view()));

                auto result = m_Collection.update_one(filter_doc.view(), update_doc.view());
                return result && result->modified_count() > 0;
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error pulling from array: " << e.what() << std::endl;
                return false;
            }
        }

        /// @brief Deletes an entire array field (or any field) from a document matching the criteria.
        /// Uses MongoDB's $unset operator. Affects the first document matched.
        /// @param match A map defining the criteria to find the target document.
        /// @param arrayField The name of the field to delete.
        /// @return True if the update was successful and one document was modified, false otherwise or on error.
        bool DeleteArrayField(const std::unordered_map<std::string, FieldValue> &match, const std::string &arrayField)
        {
            try
            {
                if (match.empty())
                {
                    std::cerr << "Error: Match criteria cannot be empty for DeleteArrayField." << std::endl;
                    return false;
                }
                bsoncxx::builder::basic::document filter_doc;
                for (const auto &[key, value] : match)
                {
                    AppendToDocument(filter_doc, key, value);
                }

                bsoncxx::builder::basic::document unset_payload_doc;
                unset_payload_doc.append(bsoncxx::builder::basic::kvp(
                    arrayField, "")); // Value for $unset doesn't matter, usually empty string or 1.

                bsoncxx::builder::basic::document update_doc;
                update_doc.append(bsoncxx::builder::basic::kvp("$unset", unset_payload_doc.view()));

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
        /// @brief The underlying `mongocxx::collection` object this wrapper manages.
        mongocxx::v_noabi::collection m_Collection;
    };
} // namespace Core::Data