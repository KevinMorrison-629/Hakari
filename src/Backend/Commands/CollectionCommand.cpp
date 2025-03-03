#include "Backend/Commands/CollectionCommand.h"

#include "Utils/Utils.h"

void Backend::Commands::collection(dpp::cluster *bot, const dpp::slashcommand_t &event, Database &db)
{
    // Define match for Rolling Player
    std::unordered_map<std::string, FieldValue> playerMatch = {
        {"player_uuid", FieldValue(FieldType::FT_STRING, event.command.member.user_id.str())}};

    std::vector<CardObject> cards = db.m_Collection_CardObject.GetAllSorted(playerMatch, "expected_value", false);

    dpp::message msg;
    dpp::embed embed;
    embed.set_title("Collection");
    embed.set_color(dpp::colors::purple_amethyst); // Choose a color
    // Add each object as a field in the embed
    std::string collection_lines = "";
    for (const auto &obj : cards)
    {
        std::string line = "| **#" + obj.getUuid() + "** | " + obj.getName() + " | iss#" +
                           std::to_string(obj.getIssueNumber()) + " | " + std::to_string(obj.getStarNumber()) + "✩ | " +
                           std::to_string(obj.getExpectedValue()) + "✨ |\n";
        collection_lines += line;
    }
    embed.add_field("", collection_lines, false);
    msg.add_embed(embed);
    event.reply(msg);
}