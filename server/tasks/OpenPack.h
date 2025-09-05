#pragma once

#include "server/data/DataService.h"
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
            Data::CardObject new_obj;
            new_obj.ownerId = player.get_id();
            new_obj.cardReferenceId = card_ref.get_id();
            new_obj.number = ++card_ref.numAcquired;
            new_obj.quality = 1;

            data_service.card_objects.insert_one(new_obj);
            new_card_objects.push_back(new_obj);

            data_service.card_references.replace_one(QDB::Query::by_id(card_ref.get_id()), card_ref);

            player.inventory.push_back(new_obj.get_id());
        }

        // 3. Add new cards to player's inventory and update.
        data_service.players.replace_one(QDB::Query::by_id(player.get_id_str()), player);

        return {true, "Pack opened successfully!", random_cards, new_card_objects};
    }
} // namespace Core::Tasks