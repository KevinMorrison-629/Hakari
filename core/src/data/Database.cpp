#include "core/data/Database.h"
#include "core/data/ScopedClient.h" // Make sure to include the new header

namespace Core::Data
{
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