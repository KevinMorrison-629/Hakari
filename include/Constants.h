#pragma once

#include <cstdint>
#include <map>
#include <string>

namespace Constants
{
    enum class CardTier
    {
        COMMON,
        RARE,
        EPIC,
        ILLUSTRATION,
        SERIES,
        EXCLUSIVE
    };

    enum class UUIDTypeEnum
    {
        SERVER,
        PLAYER,
        CHARACTER,
        CARD,
        CARD_OBJECT,
        CONSUMABLE,
        CONSUMABLE_OBJECT,
        UNDEFINED
    };

    // Game constants
    constexpr uint32_t TIER_RANGE_COMMON = 660'000'000U;       ///< @brief Roll Rate | 66%
    constexpr uint32_t TIER_RANGE_RARE = 910'000'000U;         ///< @brief Roll Rate | 25%
    constexpr uint32_t TIER_RANGE_EPIC = 970'000'000U;         ///< @brief Roll Rate | 6%
    constexpr uint32_t TIER_RANGE_ILLUSTRATION = 999'500'000U; ///< @brief Roll Rate | 2.95%
    constexpr uint32_t TIER_RANGE_SERIES = 999'999'999U;       ///< @brief Roll Rate | 0.05%
    constexpr uint32_t TIER_RANGE_EXCLUSIVE = 1'000'000'000U;  ///< @brief Roll Rate | 0.0000001% (1 / 1 billion)

    /// @note The Exclusive Card Tier allows for a player to request a given character/image to be featured on this card.
    /// This card cannot be reforged

    class UUIDType
    {
        const UUIDTypeEnum m_en;
        static inline const std::map<std::string, UUIDTypeEnum> m_StringToEnumMap = {{"sr", UUIDTypeEnum::SERVER},
                                                                                     {"pl", UUIDTypeEnum::PLAYER},
                                                                                     {"ch", UUIDTypeEnum::CHARACTER},
                                                                                     {"ca", UUIDTypeEnum::CARD},
                                                                                     {"ob", UUIDTypeEnum::CARD_OBJECT},
                                                                                     {"cn", UUIDTypeEnum::CONSUMABLE},
                                                                                     {"us", UUIDTypeEnum::CONSUMABLE_OBJECT},
                                                                                     {"xx", UUIDTypeEnum::UNDEFINED}};

    public:
        UUIDType(UUIDTypeEnum e) : m_en(e) {};
        UUIDType(const std::string &s) : UUIDType(string_to_enum(s)) {};
        std::string str() const { return enum_to_string(m_en); };
        UUIDTypeEnum enumerate() const { return m_en; };

    private:
        UUIDTypeEnum string_to_enum(const std::string &s) const
        {
            if (m_StringToEnumMap.find(s) != m_StringToEnumMap.end())
            {
                return m_StringToEnumMap.at(s);
            }
            else
            {
                return UUIDTypeEnum::UNDEFINED;
            };
        }

        std::string enum_to_string(const UUIDTypeEnum &e) const
        {
            // look through the enum map to get the string from the enum
            for (const auto iter : m_StringToEnumMap)
            {
                if (iter.second == e)
                {
                    return iter.first;
                }
            }
            return "xx";
        };
    };

    // Add more constants as needed...
} // namespace Constants