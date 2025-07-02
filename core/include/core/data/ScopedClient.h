#pragma once

#include "core/data/CollectionWrapper.h" // Corrected path
#include "core/data/collection/Card.h"
#include "core/data/collection/Player.h"

#include <mongocxx/pool.hpp> // Required for mongocxx::pool::entry

namespace Core::Data
{
    /// @brief Manages a MongoDB client connection leased from a connection pool.
    /// This class ensures that the client connection is automatically returned to the pool
    /// when this object goes out of scope. It also provides convenient access to
    /// typed collections.
    class ScopedClient
    {
    public:
        /// @brief Constructs a ScopedClient, acquiring a client from the pool.
        /// @param client A mongocxx::pool::entry representing the leased client connection.
        /// @param db_name The name of the database to interact with.
        ScopedClient(mongocxx::pool::entry client, const std::string &db_name)
            : m_Client(std::move(client)),
              // Initialize your collection wrappers here, using the acquired client
              Cards{(*m_Client)[db_name]["cards"]}
              // TODO: Initialize Player collection if it's intended to be used.
              // Players{(*m_Client)[db_name]["players"]}
        {
        }

        /// @brief Publicly accessible wrapper for the 'cards' collection.
        CollectionWrapper<Collection::Card> Cards;
        // TODO: Add Player collection wrapper if needed
        // CollectionWrapper<Collection::Player> Players;

    private:
        /// @brief Holds the client connection leased from the pool.
        /// When this ScopedClient object is destroyed, the connection is automatically returned to the pool.
        mongocxx::pool::entry m_Client;
    };
}