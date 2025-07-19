#include "server/data/Database.h"
#include "server/data/ScopedClient.h" // Make sure to include the new header

namespace Core::Data
{
    /// @brief Acquires a database client from the pool.
    /// This function is called by the Database class to provide a client
    /// for database operations. It ensures that the database is connected
    /// before attempting to acquire a client.
    /// @return A ScopedClient object that provides access to the database.
    /// @throws std::runtime_error if the database is not connected.
    ScopedClient Database::getClient()
    {
        if (!m_Pool)
        {
            throw std::runtime_error("Database is not connected. Call Connect() first.");
        }
        // Acquire a client from the pool and use it to construct a ScopedClient
        return ScopedClient(m_Pool->acquire(), m_DatabaseName);
    }
}