#include "Backend/Commands/DropCommand.h"

#include "Constants.h"
#include "Utils/Logger.h"
#include "Utils/Utils.h"

#include "Persistence/Field.h"

void Backend::Commands::drop(dpp::cluster *bot, const dpp::slashcommand_t &event, Database &db)
{
    // Get Player Id
    std::string playerUuid = event.command.member.user_id.str();

    // Get Random UUID
    std::string randomCode = db.GetRandomUniqueCode(
        Constants::UUIDType(Constants::UUIDTypeEnum::CARD_OBJECT)); // Unique ID to assign to the rolled card

    // Get a Random Tier
    Constants::CardTier tier = GetRandomCardTier();
    // Get a Ranomd Star Level
    int32_t star = GetRandomStarLevel();

    // Select a Random Card with the given Tier
    std::vector<Card> cards = db.m_Collection_Card.GetRandom(
        1U, {{std::string("tier"), FieldValue(FieldType::FT_INT_32, static_cast<int32_t>(tier))}});

    // successfully found card in tier
    if (cards.size() == 1)
    {
        Card card = cards[0];

        // Define match for Character (need to get the character rank)
        std::unordered_map<std::string, FieldValue> charMatch = {
            {"_id", FieldValue(FieldType::FT_OBJECT_ID, card.getCharacterId())}};
        Character character =
            db.m_Collection_Character.GetRandom(1, charMatch)[0]; // should always give back the same character

        int32_t expectedValue = static_cast<int32_t>(
            100.0 + 10.0 * std::pow(2.0, static_cast<int32_t>(tier)) *
                        std::pow(2.0, std::max(0.0, (5000.0 - static_cast<double>(character.getRank())) / 5000.0) * star));

        // Create Card Object
        CardObject obj;
        obj.setUuid(randomCode);
        obj.setCardId(card.getId());
        obj.setName(card.getName());
        obj.setIssueNumber(card.getNumAcquired() + 1);
        obj.setStarNumber(star);
        obj.setExpectedValue(expectedValue);
        obj.setOwningPlayerUuid(playerUuid);

        // Add card to the database
        auto _id = db.m_Collection_CardObject.Add(obj);
        if (_id)
        {
            // Define match for Rolling Player
            std::unordered_map<std::string, FieldValue> playerMatch = {
                {"uuid", FieldValue(FieldType::FT_STRING, playerUuid)}};

            // Add card to player inventory
            db.m_Collection_Player.PushToArray(playerMatch, "collection",
                                               FieldValue(FieldType::FT_OBJECT_ID, _id.value().to_string()));

            // Update the number of issue this card has had
            std::unordered_map<std::string, FieldValue> cardMatch = {
                {"_id", FieldValue(FieldType::FT_OBJECT_ID, card.getId())}};
            if (db.m_Collection_Card.UpdateEntry(
                    cardMatch, {{"num_acquired", FieldValue(FieldType::FT_INT_32, card.getNumAcquired() + 1)}}))
            {
                Utils::Logger::Log(Utils::Logger::Type::INFO, "Updated Entry (num_aquired)");
            }
        }

        // Create Embed
        dpp::message msg;
        dpp::embed embed;
        embed.title = card.getName();
        embed.description = std::to_string(card.getTierId()) + " | " + std::to_string(expectedValue);
        std::string img_url =
            "https://kevinmorrison-629.github.io/Hakari/images/" + card.getCharacterId() + "/" + card.getImageUrl();

        embed.set_image(img_url);
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

int32_t Backend::Commands::GetRandomStarLevel()
{
    // Random Roll (0->1'000'000'000)
    uint32_t star = Utils::RandomStar();

    if (star < Constants::STAR_RANGE_ZERO)
    {
        return 0;
    }
    else if (star < Constants::STAR_RANGE_ONE)
    {
        return 1;
    }
    else if (star < Constants::STAR_RANGE_TWO)
    {
        return 2;
    }
    else if (star < Constants::STAR_RANGE_THREE)
    {
        return 3;
    }
    else if (star < Constants::STAR_RANGE_FOUR)
    {
        return 4;
    }
    else if (star == Constants::STAR_RANGE_FIVE)
    {
        return 5;
    }
    else
    {
        // Shouldnt Get Here, but going to return 0 Stars here just to ensure
        return 0;
    }
}
