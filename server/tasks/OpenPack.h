#pragma once

#include "server/data/DataService.h"
#include <ctime>
#include <memory>
#include <string>
#include <vector>

namespace Core::Tasks
{
    struct PackOpeningResult
    {
        bool success = false;
        std::string message;
        std::vector<Core::Data::CardReference> opened_card_refs;
        std::vector<Core::Data::CardObject> opened_card_objs;
    };

    /**
     * @brief Core logic for opening a pack. It now uses the DataService for all DB access.
     * @param data_service The central service for database interaction.
     * @param player_id The unique database ObjectId of the player.
     * @return A PackOpeningResult struct with the outcome.
     */
    inline PackOpeningResult OpenPackForPlayer(Data::DataService &data_service, Core::Data::Player &player)
    {
        // 1. Get 3 random card references.
        auto random_cards = data_service.card_references.find_random(QDB::Query(), 1, false);
        if (random_cards.size() < 1)
        {
            return {false, "Not enough unique cards in the game to open a pack!"};
        }

        std::vector<Data::CardObject> new_card_objects;

        // 2. Create new CardObject instances.
        for (auto &card_ref : random_cards)
        {
            time_t rawtime;
            time(&rawtime); // Get current time

            Data::CardObject new_obj;
            new_obj.ownerId = player.get_id();
            new_obj.cardReferenceId = card_ref.get_id();
            new_obj.number = ++card_ref.numAcquired;
            new_obj.attackPoints = 0;
            new_obj.healthPoints = 0;
            new_obj.lastAcquisitionDate = bsoncxx::types::b_timestamp(rawtime);
            new_obj.ownerHistory = {player.get_id()};

            new_card_objects.push_back(new_obj);

            // 3. Insert the Card Object into the database collection
            data_service.card_objects.insert_one(new_obj);

            // 3. Increment the Card Reference numAcquired value
            data_service.card_references.update_one(QDB::Query::by_id(card_ref.get_id()),
                                                    QDB::Update().inc("numAcquired", 1));
            // 4. Add new cards to player's inventory and update.
            data_service.players.update_one(QDB::Query::by_id(player.get_id_str()),
                                            QDB::Update().push("cards", new_obj.get_id()));
        }

        return {true, "Pack opened successfully!", random_cards, new_card_objects};
    }
} // namespace Core::Tasks