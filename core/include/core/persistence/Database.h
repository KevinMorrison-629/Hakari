#pragma once

#include <string>
#include <memory>

#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

#include "core/persistence/ScopedClient.h"

namespace Core::Persistence
{
    class Database
    {
    public:
        // Delete copy and move constructors for singleton pattern
        Database(const Database &) = delete;
        Database &operator=(const Database &) = delete;

        static Database &instance()
        {
            static Database db_instance;
            return db_instance;
        }

        void Connect(const std::string &connectionString, const std::string &dbName)
        {
            m_DatabaseName = dbName;
            // The mongocxx::instance is still required and managed automatically
            m_Pool = std::make_unique<mongocxx::pool>(mongocxx::uri{connectionString});
        }

        ScopedClient getClient();

        const std::string &getDatabaseName() const { return m_DatabaseName; }

    private:
        // Private constructor for singleton pattern
        Database() = default;
        ~Database() = default;

        inline static mongocxx::instance m_Instance{};
        std::unique_ptr<mongocxx::pool> m_Pool;
        std::string m_DatabaseName;
    };
}