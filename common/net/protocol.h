#pragma once

#include <cstdint>
#include <vector>

namespace Core::Net
{
    // Master list of all message types
    enum class MessageType : uint8_t
    {
        // Invalid Type
        INVALID_TYPE = 0,

        // Time Critical (low-latency, unreliable)
        C2S_PLAYER_INPUT = 1,
        S2C_GAME_STATE = 2,

        // Time Deferrable (time-tolerant, reliable)
        S2C_INITIALIZE_WORLD = 128,

        C2S_REQUEST_INVENTORY = 129,
        S2C_RESPONSE_INVENTORY = 130,
        C2S_REQUEST_OPEN_PACK = 131,
        S2C_RESPONSE_OPEN_PACK = 132,
    };

    // Helper function to get message type from a raw buffer
    inline MessageType PeekMessageType(const std::vector<uint8_t> &data)
    {
        if (data.empty())
        {
            // Handle error, maybe return an invalid type
            return MessageType::INVALID_TYPE;
        }
        return *reinterpret_cast<const MessageType *>(data[0]);
    }
} // namespace Core::Net