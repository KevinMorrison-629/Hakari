#pragma once

#include "core/data/collectionWrapper.h"
#include "core/data/collection/Card.h"
#include "core/data/collection/Player.h"

#include <mongocxx/pool.hpp> // Required for mongocxx::pool::entry

namespace Core::Data
{
    class ScopedClient
    {
    public:
        // Constructor acquires a client from the pool
        ScopedClient(mongocxx::pool::entry client, const std::string &db_name)
            : m_Client(std::move(client)),
              // Initialize your collection wrappers here, using the acquired client
              Cards{(*m_Client)[db_name]["cards"]}
        {
        }

        // Public members for easy access to your collections
        CollectionWrapper<Collection::Card> Cards;

    private:
        mongocxx::pool::entry m_Client; // This holds the client connection. When this object is destroyed, the connection is returned to the pool.
    };
}