
#ifndef HAKARI_ORM_H
#define HAKARI_ORM_H

#include <tuple>
#include <string>
#include <sqlite3.h>

namespace HakariORM
{
    // Define an example class with its table name and members
    struct Image
    {
        static constexpr const char *tableName = "Images";
        std::string url;
        int image_height;
        int image_width;
    };

    // Define an example class with its table name and members
    struct User
    {
        static constexpr const char *tableName = "Users";
        std::string username;
        int age;
        Image user_profile_img;
    };

    // Class responsible for interacting with the SQLite database
    class Repository
    {
    private:
        static sqlite3 *db;

        // Define a generic mapping for columns
        template <typename... Columns>
        struct TableMapping
        {
            static constexpr size_t column_count = sizeof...(Columns);
            std::tuple<Columns...> columns;
        };

    public:
        Repository(const char *dbFile);

        ~Repository();

        // Function to insert an object into the database
        template <typename T>
        void insert(const T &obj);

        // Function to fetch an object from the database based on a condition
        template <typename T>
        T fetchBy(const std::string &condition);

    private:
        // Helper function to handle nested classes
        template <typename T>
        constexpr auto mapToTable();

        // Function to concatenate mappings recursively
        template <typename... Mappings>
        constexpr auto concatenateMappings(const Mappings &...mappings);

        // Function to create a mapping for a class
        template <typename T>
        constexpr auto createMapping();

        // Function to create a mapping for multiple classes
        template <typename... Ts>
        constexpr auto createMapping();

        // Function to execute a SQL query
        void executeQuery(const std::string &query);

        // Function to generate an INSERT query for an object
        template <typename T, typename... Columns>
        std::string generateInsertQuery(const T &obj, const TableMapping<Columns...> &mapping);

        // Function to generate a SELECT query for an object based on a condition
        template <typename T>
        std::string generateFetchQuery(const std::string &condition);

        // Function to execute a fetch query and return the result as an object
        template <typename T>
        T executeFetchQuery(const std::string &query, const TableMapping<> &mapping);

        // Function to get the value of a column from a result set
        template <typename T>
        auto getColumnValue(sqlite3_stmt *stmt, const std::string &columnName);
    };

}

#endif