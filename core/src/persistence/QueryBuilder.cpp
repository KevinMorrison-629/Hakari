
#include "core/persistence/QueryBuilder.h"
#include <bsoncxx/builder/basic/array.hpp>

namespace Core::Persistence
{
    Query &Query::Equals(const std::string &key, const FieldValue &value)
    {
        // A simple key-value pair is the most common equality check.
        AppendToDocument(m_builder, key, value);
        return *this;
    }

    Query &Query::Or(const std::vector<Query> &queries)
    {
        bsoncxx::builder::basic::array or_array;
        for (const auto &q : queries)
        {
            // Append each sub-query's BSON document to the $or array
            or_array.append(q.build().view());
        }
        m_builder.append(bsoncxx::builder::basic::kvp("$or", or_array.view()));
        return *this;
    }

    Query &Query::NotEquals(const std::string &key, const FieldValue &value)
    {
        return addComparison("$ne", key, value);
    }

    Query &Query::GreaterThan(const std::string &key, const FieldValue &value)
    {
        return addComparison("$gt", key, value);
    }

    Query &Query::GreaterThanOrEqual(const std::string &key, const FieldValue &value)
    {
        return addComparison("$gte", key, value);
    }

    Query &Query::LessThan(const std::string &key, const FieldValue &value)
    {
        return addComparison("$lt", key, value);
    }

    Query &Query::LessThanOrEqual(const std::string &key, const FieldValue &value)
    {
        return addComparison("$lte", key, value);
    }

    Query &Query::In(const std::string &key, const std::vector<FieldValue> &values)
    {
        return addArrayOperator("$in", key, values);
    }

    Query &Query::NotIn(const std::string &key, const std::vector<FieldValue> &values)
    {
        return addArrayOperator("$nin", key, values);
    }

    Query &Query::Exists(const std::string &key, bool exists)
    {
        m_builder.append(bsoncxx::builder::basic::kvp(key, [exists](bsoncxx::builder::basic::sub_document sub_doc)
                                                      { sub_doc.append(bsoncxx::builder::basic::kvp("$exists", exists)); }));
        return *this;
    }

    bsoncxx::document::value &Query::build()
    {
        // Extract a copy of the built document.
        return m_builder.extract();
    }

    Query &Query::addComparison(const std::string &op, const std::string &key, const FieldValue &value)
    {
        // This structure creates a document like: { key: { $op: value } }
        // For example: { "tier": { "$gt": 3 } }
        bsoncxx::builder::basic::document condition_doc;
        AppendToDocument(condition_doc, op, value);

        m_builder.append(bsoncxx::builder::basic::kvp(key, condition_doc.view()));
        return *this;
    }

    Query &Query::addArrayOperator(const std::string &op, const std::string &key, const std::vector<FieldValue> &values)
    {
        // This structure creates a document like: { key: { $op: [val1, val2] } }
        // For example: { "name": { "$in": ["A", "B"] } }
        bsoncxx::builder::basic::array value_array;
        for (const auto &v : values)
        {
            AppendToArray(value_array, v);
        }

        bsoncxx::builder::basic::document condition_doc;
        condition_doc.append(bsoncxx::builder::basic::kvp(op, value_array.view()));

        m_builder.append(bsoncxx::builder::basic::kvp(key, condition_doc.view()));
        return *this;
    }
}