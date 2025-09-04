#pragma once

#include "server/core/TaskManager.h"
#include "server/tasks/Tasks.h"

namespace Core::Discord
{
    inline void OpenPackDiscordCommand(const Core::Utils::TaskDiscordCommand &task)
    {
        // 1. Use the DataService's helper method to get the player.
        int64_t discord_id = static_cast<int64_t>(task.user_id);
        Core::Data::Player player = task.data_service->find_or_create_player_by_discord_id(discord_id);

        // 2. Call the core game logic, passing in the entire service.
        Core::Tasks::PackOpeningResult result = Core::Tasks::OpenPackForPlayer(*task.data_service, player);

        // 3. Format and send the Discord response (logic remains the same).
        dpp::message response_msg;
        if (result.success)
        {
            dpp::embed embed = dpp::embed().set_title("Pack Opened!").set_color(dpp::colors::green_apple);
            std::string description = "Congratulations! You received 3 new cards:\n\n";
            for (size_t i = 0; i < result.opened_card_refs.size(); ++i)
            {
                description += "• **" + result.opened_card_refs[i].name + "** (#" +
                               std::to_string(result.opened_card_objs[i].number) + ")\n";
            }
            embed.set_description(description);

            if (!result.opened_card_refs.empty())
            {
                std::string url = "https://hotpink-octopus-624350.hostingersite.com/character/" +
                                  result.opened_card_refs[0].characterId.to_string();
                std::cout << url << std::endl;
                embed.set_image(url);
            }
            response_msg.add_embed(embed);
        }
        else
        {
            response_msg.set_content(result.message);
        }

        task.bot_cluster->interaction_response_edit(task.interaction_token, response_msg);
    }
} // namespace Core::Discord