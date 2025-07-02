#pragma once

#include <cstdint>
#include <map>
#include <string>

/// @brief Namespace containing various game-related constants and utility types.
namespace Constants
{
    /// @brief Defines the rarity tiers for cards.
    enum class CardTier
    {
        COMMON,       ///< Common card tier.
        RARE,         ///< Rare card tier.
        EPIC,         ///< Epic card tier.
        ILLUSTRATION, ///< Illustration card tier.
        SERIES,       ///< Series-specific card tier.
        EXCLUSIVE     ///< Exclusive, potentially unique card tier.
    };

    /// @brief Defines the types of entities that can be identified by a UUID.
    enum class UUIDTypeEnum
    {
        SERVER,            ///< Identifier for a server instance.
        PLAYER,            ///< Identifier for a player.
        CHARACTER,         ///< Identifier for a game character.
        CARD,              ///< Identifier for a card definition or type.
        CARD_OBJECT,       ///< Identifier for a specific instance of a card owned by a player.
        CONSUMABLE,        ///< Identifier for a type of consumable item.
        CONSUMABLE_OBJECT, ///< Identifier for a specific instance of a consumable item.
        UNDEFINED          ///< Undefined or unknown UUID type.
    };

    // Game constants
    /// @brief Defines the upper bound for rolling a COMMON card. Roll Rate | 66%
    constexpr uint32_t TIER_RANGE_COMMON = 660'000'000U;
    /// @brief Defines the upper bound for rolling a RARE card. Roll Rate | 25%
    constexpr uint32_t TIER_RANGE_RARE = 910'000'000U;
    /// @brief Defines the upper bound for rolling an EPIC card. Roll Rate | 6%
    constexpr uint32_t TIER_RANGE_EPIC = 970'000'000U;
    /// @brief Defines the upper bound for rolling an ILLUSTRATION card. Roll Rate | 2.95%
    constexpr uint32_t TIER_RANGE_ILLUSTRATION = 999'500'000U;
    /// @brief Defines the upper bound for rolling a SERIES card. Roll Rate | 0.05%
    constexpr uint32_t TIER_RANGE_SERIES = 999'999'999U;
    /// @brief Defines the upper bound for rolling an EXCLUSIVE card. Roll Rate | 0.0000001% (1 / 1 billion)
    constexpr uint32_t TIER_RANGE_EXCLUSIVE = 1'000'000'000U;

    /// @note The Exclusive Card Tier allows for a player to request a given character/image to be featured on this card.
    /// This card cannot be reforged.

    /// @brief A utility class for handling UUID types, allowing conversion between enum and string representations.
    /// UUID types are typically used as prefixes in string UUIDs to indicate the type of entity the UUID refers to.
    class UUIDType
    {
    private:
        /// @brief The underlying enum value for the UUID type.
        const UUIDTypeEnum m_en;
        /// @brief Static map for converting string prefixes to UUIDTypeEnum values.
        static inline const std::map<std::string, UUIDTypeEnum> m_StringToEnumMap = {{"sr", UUIDTypeEnum::SERVER},
                                                                                     {"pl", UUIDTypeEnum::PLAYER},
                                                                                     {"ch", UUIDTypeEnum::CHARACTER},
                                                                                     {"ca", UUIDTypeEnum::CARD},
                                                                                     {"ob", UUIDTypeEnum::CARD_OBJECT},
                                                                                     {"cn", UUIDTypeEnum::CONSUMABLE},
                                                                                     {"us", UUIDTypeEnum::CONSUMABLE_OBJECT},
                                                                                     {"xx", UUIDTypeEnum::UNDEFINED}};

    public:
        /// @brief Constructor from a UUIDTypeEnum value.
        /// @param e The enum value.
        UUIDType(UUIDTypeEnum e) : m_en(e){};

        /// @brief Constructor from a string prefix.
        /// Converts the string to the corresponding UUIDTypeEnum.
        /// @param s The string prefix (e.g., "pl" for PLAYER).
        UUIDType(const std::string &s) : UUIDType(string_to_enum(s)){};

        /// @brief Gets the string prefix representation of the UUID type.
        /// @return The string prefix (e.g., "pl"). Returns "xx" if the type is undefined or not found.
        std::string str() const { return enum_to_string(m_en); };

        /// @brief Gets the enum representation of the UUID type.
        /// @return The UUIDTypeEnum value.
        UUIDTypeEnum enumerate() const { return m_en; };

    private:
        /// @brief Converts a string prefix to its corresponding UUIDTypeEnum value.
        /// @param s The string prefix.
        /// @return The UUIDTypeEnum value. Returns UUIDTypeEnum::UNDEFINED if the string is not a valid prefix.
        UUIDTypeEnum string_to_enum(const std::string &s) const
        {
            auto it = m_StringToEnumMap.find(s);
            if (it != m_StringToEnumMap.end())
            {
                return it->second;
            }
            else
            {
                return UUIDTypeEnum::UNDEFINED;
            };
        }

        /// @brief Converts a UUIDTypeEnum value to its string prefix.
        /// @param e The UUIDTypeEnum value.
        /// @return The string prefix. Returns "xx" if the enum value is not found in the map.
        std::string enum_to_string(const UUIDTypeEnum &e) const
        {
            // look through the enum map to get the string from the enum
            for (const auto &iter : m_StringToEnumMap)
            {
                if (iter.second == e)
                {
                    return iter.first;
                }
            }
            return "xx"; // Default for undefined or unmapped enums
        };
    };

    // Add more constants as needed...
} // namespace Constants