#pragma once

#include <string>
#include <memory>

#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

#include "core/data/ScopedClient.h"

namespace Core::Data
{
    /// @brief Represents the main database interface, implemented as a singleton.
    /// This class manages the connection pool to the MongoDB database.
    class Database
    {
    public:
        // Delete copy and move constructors for singleton pattern
        Database(const Database &) = delete;
        Database &operator=(const Database &) = delete;

        /// @brief Gets the singleton instance of the Database.
        /// @return Reference to the singleton Database instance.
        static Database &instance()
        {
            static Database db_instance;
            return db_instance;
        }

        /// @brief Connects to the MongoDB database.
        /// @param connectionString The MongoDB connection string.
        /// @param dbName The name of the database to use.
        void Connect(const std::string &connectionString, const std::string &dbName)
        {
            m_DatabaseName = dbName;
            // The mongocxx::instance is still required and managed automatically
            m_Pool = std::make_unique<mongocxx::pool>(mongocxx::uri{connectionString});
        }

        /// @brief Acquires a database client from the pool.
        /// @return A ScopedClient object that provides access to the database.
        /// @throws std::runtime_error if the database is not connected.
        ScopedClient getClient();

        /// @brief Gets the name of the current database.
        /// @return The name of the database.
        const std::string &getDatabaseName() const { return m_DatabaseName; }

    private:
        /// @brief Private constructor for the singleton pattern.
        Database() = default;
        /// @brief Private destructor.
        ~Database() = default;

        /// @brief Static mongocxx instance, managed automatically.
        inline static mongocxx::instance m_Instance{};
        /// @brief Connection pool for MongoDB clients.
        std::unique_ptr<mongocxx::pool> m_Pool;
        /// @brief Name of the database.
        std::string m_DatabaseName;
    };
}