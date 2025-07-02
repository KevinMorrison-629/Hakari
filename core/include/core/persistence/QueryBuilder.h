#pragma once

#include "core/persistence/Field.h" // For FieldValue

#include <bsoncxx/document/value.hpp>

namespace Core::Persistence
{
    class Query
    {
    public:
        Query() = default;

        // --- Logical Operators ---
        Query &Or(const std::vector<Query> &queries);

        // --- Comparison Operators ---
        Query &Equals(const std::string &key, const FieldValue &value);
        Query &NotEquals(const std::string &key, const FieldValue &value);
        Query &GreaterThan(const std::string &key, const FieldValue &value);
        Query &GreaterThanOrEqual(const std::string &key, const FieldValue &value);
        Query &LessThan(const std::string &key, const FieldValue &value);
        Query &LessThanOrEqual(const std::string &key, const FieldValue &value);
        Query &In(const std::string &key, const std::vector<FieldValue> &values);
        Query &NotIn(const std::string &key, const std::vector<FieldValue> &values);

        // --- Element Operators ---
        Query &Exists(const std::string &key, bool exists = true);

        bsoncxx::document::value &build();

    private:
        // Helper for comparison operators like $gt, $ne, etc.
        Query &addComparison(const std::string &op, const std::string &key, const FieldValue &value);

        // Helper for array operators like $in, $nin, etc.
        Query &addArrayOperator(const std::string &op, const std::string &key, const std::vector<FieldValue> &values);

        bsoncxx::builder::basic::document m_builder;
    };
}