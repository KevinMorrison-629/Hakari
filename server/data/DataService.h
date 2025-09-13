#pragma once

#include "common/data/Dataclasses.h"
#include "quickdb/quickdb.h"
#include <memory>

namespace Core::Data
{
    /**
     * @brief A service class that centralizes all database collections and
     * provides helper methods for common database operations. This is the preferred
     * way to interact with the database.
     */
    class DataService
    {
    public:
        // --- Public Collection Handles ---
        // Direct access to the collection objects for maximum flexibility.
        QDB::Collection<Player> players;
        QDB::Collection<CardReference> card_references;
        QDB::Collection<CardObject> card_objects;
        QDB::Collection<AbilityReference> ability_references;

        /**
         * @brief Constructs the service and initializes all collection handles.
         * @param db A shared pointer to the database connection pool.
         */
        DataService(std::shared_ptr<QDB::Database> db)
            : players(db->get_collection<Player>("hakari", "players")),
              card_references(db->get_collection<CardReference>("hakari", "card_references")),
              card_objects(db->get_collection<CardObject>("hakari", "card_objects")),
              ability_references(db->get_collection<AbilityReference>("hakari", "card-abilities"))
        {
        }

        // --- Helper Methods for Common Queries ---

        /**
         * @brief Finds a player by their unique Discord ID. If no player is found,
         * a new player document is created and inserted into the database.
         * @param discord_id The Discord user's snowflake ID.
         * @return The found or newly created Player object.
         */
        Player find_or_create_player_by_discord_id(int64_t discord_id)
        {
            auto query = QDB::Query().eq("discord_id", discord_id);
            std::optional<Player> player_opt = players.find_one(query);

            if (player_opt)
            {
                return *player_opt;
            }
            else
            {
                Player new_player;
                new_player.discord_id = discord_id;
                players.insert_one(new_player);
                return new_player;
            }
        }

        std::optional<Player> find_player_by_email(std::string email)
        {
            auto query = QDB::Query().eq("email", email);
            return players.find_one(query);
        }

        /**
         * @brief Finds a player by their unique display name.
         * @param display_name The display name to search for (case-insensitive).
         * @return An optional containing the Player if found.
         * @note Assumes the 'Player' dataclass has a 'display_name' field.
         */
        std::optional<Player> find_player_by_display_name(const std::string &display_name)
        {
            auto query = QDB::Query().eq("display_name", display_name);
            return players.find_one(query);
        }

        std::optional<Player> find_player_by_id(const std::string &id)
        {
            auto query = QDB::Query().eq("_id", bsoncxx::oid(id));
            return players.find_one(query);
        }
    };

} // namespace Core::Data