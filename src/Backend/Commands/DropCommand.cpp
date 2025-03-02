#include "Backend/Commands/DropCommand.h"

#include "Constants.h"
#include "Utils/Logger.h"
#include "Utils/Utils.h"

#include "Persistence/Field.h"

void Backend::Commands::drop(dpp::cluster *bot, const dpp::slashcommand_t &event, Database &db)
{
    // Get Random UUID
    std::string randomCode = db.GetRandomUniqueCode(
        Constants::UUIDType(Constants::UUIDTypeEnum::CARD_OBJECT)); // Unique ID to assign to the rolled card

    // Get a Random Tier
    Constants::CardTier tier = GetRandomCardTier();

    // Select a Random Card with the given Tier
    std::vector<Card> cards = db.m_Collection_Card.GetRandom(
        1U, {{std::string("tier"), FieldValue(FieldType::FT_INT_32, static_cast<int32_t>(tier))}});

    // successfully found card in tier
    if (cards.size() == 1)
    {
        Card card = cards[0];

        // Create Card Object
        CardObject obj;
        obj.setUuid(randomCode);
        obj.setCardId(card.getId());
        obj.setIssueNumber(card.getNumAcquired() + 1);

        // Add card to the database
        auto _id = db.m_Collection_CardObject.Add(obj);
        if (_id)
        {
            // Define match for Rolling Player
            std::unordered_map<std::string, FieldValue> playerMatch = {
                {"uuid", FieldValue(FieldType::FT_STRING, event.command.member.user_id.str())}};

            // Add card to player inventory
            db.m_Collection_Player.PushToArray(playerMatch, "collection",
                                               FieldValue(FieldType::FT_OBJECT_ID, _id.value().to_string()));
        }

        // Create Embed
        dpp::message msg;
        dpp::embed embed;
        embed.title = card.getName();
        embed.description = std::to_string(card.getTierId());
        embed.set_image("https://raw.githubusercontent.com/KevinMorrison-629/HakaribotImages/refs/heads/main/images/" +
                        card.getImageUrl());
        msg.add_embed(embed);
        event.reply(msg);
    }
    else
    {
        event.reply("Could not find any cards");
    }
}

Constants::CardTier Backend::Commands::GetRandomCardTier()
{
    // Random Roll (0->1'000'000'000)
    uint32_t roll = Utils::RandomRoll();

    if (roll < Constants::TIER_RANGE_COMMON)
    {
        return Constants::CardTier::COMMON;
    }
    else if (roll < Constants::TIER_RANGE_RARE)
    {
        return Constants::CardTier::RARE;
    }
    else if (roll < Constants::TIER_RANGE_EPIC)
    {
        return Constants::CardTier::EPIC;
    }
    else if (roll < Constants::TIER_RANGE_ILLUSTRATION)
    {
        return Constants::CardTier::ILLUSTRATION;
    }
    else if (roll < Constants::TIER_RANGE_SERIES)
    {
        return Constants::CardTier::SERIES;
    }
    else if (roll == Constants::TIER_RANGE_EXCLUSIVE)
    {
        return Constants::CardTier::EXCLUSIVE;
    }
    else
    {
        // Shouldnt Get Here, but going to return CommonCard Tier here just to ensure
        return Constants::CardTier::COMMON;
    }
}