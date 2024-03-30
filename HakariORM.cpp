
#include "HakariORM.h"

#include <iostream>

namespace HakariORM
{
    class Repository
    {
    private:
        static sqlite3 *db;

        // Helper function to handle nested classes
        template <typename T>
        constexpr auto mapToTable()
        {
            // If T is a class, return its TableMapping
            if constexpr (std::is_class_v<T>)
            {
                return HakariORM::Repository::TableMapping<T>{};
            }
            else
            {
                // Otherwise, return T itself
                return T{};
            }
        }

        // Define a generic mapping for columns
        template <typename... Columns>
        struct TableMapping
        {
            static constexpr size_t column_count = sizeof...(Columns);
            std::tuple<Columns...> columns;
        };

        // Function to concatenate mappings recursively
        template <typename... Mappings>
        constexpr auto concatenateMappings(const Mappings &...mappings)
        {
            return std::tuple_cat(mappings.columns...);
        }

        // Function to create a mapping for a class
        template <typename T>
        constexpr auto createMapping()
        {
            return mapToTable<T>();
        }

        // Function to create a mapping for multiple classes
        template <typename... Ts>
        constexpr auto createMapping()
        {
            return concatenateMappings(mapToTable<Ts>()...);
        }

        // Function to create a mapping for a class
        template <typename T>
        constexpr auto createMapping()
        {
            return mapToTable<T>();
        }

        // Function to create a mapping for multiple classes
        template <typename... Ts>
        constexpr auto createMapping()
        {
            return concatenateMappings(mapToTable<Ts>()...);
        }

    public:
        Repository(const char *dbFile)
        {

            int rc = sqlite3_open(dbFile, &db);
            if (rc)
            {
                std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_close(db);
                exit(1);
            }
        }

        ~Repository()
        {
            sqlite3_close(db);
        }

        // Function to insert an object into the database
        template <typename T>
        void insert(const T &obj)
        {
            std::string query = generateInsertQuery(obj, createMapping<T>());
            executeQuery(query);
        }

        // Function to fetch an object from the database based on a condition
        template <typename T>
        T fetchBy(const std::string &condition)
        {
            std::string query = generateFetchQuery<T>(condition);
            return executeFetchQuery<T>(query, createMapping<T>());
        }

    private:
        // Function to execute a SQL query
        void executeQuery(const std::string &query)
        {
            char *errMsg;
            int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
            if (rc != SQLITE_OK)
            {
                std::cerr << "SQL error: " << errMsg << std::endl;
                sqlite3_free(errMsg);
            }
        }

        // Function to generate an INSERT query for an object
        template <typename T, typename... Columns>
        std::string generateInsertQuery(const T &obj, const TableMapping<Columns...> &mapping)
        {
            std::string query = "INSERT INTO " + std::string(T::tableName) + " (";
            std::string values = "VALUES (";

            auto addColumn = [&](const std::string &name)
            {
                query += name + ",";
                values += "?,";
            };

            std::apply([&](const auto &...columns)
                       { (addColumn(columns.name), ...); },
                       mapping.columns);

            query.pop_back();  // Remove the last comma
            values.pop_back(); // Remove the last comma
            query += ") " + values + ");";

            return query;
        }

        // Function to generate a SELECT query for an object based on a condition
        template <typename T>
        std::string generateFetchQuery(const std::string &condition)
        {
            return "SELECT * FROM " + std::string(T::tableName) + " WHERE " + condition + ";";
        }

        // Function to execute a fetch query and return the result as an object
        template <typename T>
        T executeFetchQuery(const std::string &query, const TableMapping<> &mapping)
        {
            sqlite3_stmt *stmt;
            sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
            sqlite3_step(stmt);

            T obj;
            std::apply([&](auto &...columns)
                       { ((columns.value = getColumnValue<T>(stmt, columns.name)), ...); },
                       mapping.columns);

            sqlite3_finalize(stmt);
            return obj;
        }

        // Function to get the value of a column from a result set
        template <typename T>
        auto getColumnValue(sqlite3_stmt *stmt, const std::string &columnName)
        {
            if constexpr (std::is_same_v<T, int>)
            {
                return sqlite3_column_int(stmt, sqlite3_column_index(stmt, columnName.c_str()));
            }
            else if constexpr (std::is_same_v<T, std::string>)
            {
                return reinterpret_cast<const char *>(sqlite3_column_text(stmt, sqlite3_column_index(stmt, columnName.c_str())));
            }
            else if constexpr (std::is_class_v<T>)
            {
                // Recursively fetch nested object
                return executeFetchQuery<T>(columnName, createMapping<T>());
            }
            // Add more type mappings as needed
        }
    };
}