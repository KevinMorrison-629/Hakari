#pragma once

#include <mutex>
#include <random>
#include <string>

#include "Constants.h"

namespace Utils
{
    const std::string VALID_CODE_CHARACTERS = "abcdefghijklmnopqrstuvwxyz1234567890";

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::mutex utilsMutex;
    inline static std::uniform_int_distribution<uint32_t> random_roll_dist(0, Constants::TIER_RANGE_EXCLUSIVE);
    inline static std::uniform_int_distribution<uint32_t> random_star_dist(0, Constants::STAR_RANGE_FIVE);
    inline static std::uniform_int_distribution<uint64_t> random_code_dist(0, VALID_CODE_CHARACTERS.size());

    /// @brief Error Structure
    enum ErrorCode
    {
        SUCCESS,
        DEFAULT_ERROR,
        FILE_NOT_FOUND,
        FAILED_SQL_COMMAND,
        PYTHON_INTERPRETER_ERROR,
        UNKNOWN_ERROR
    };

    /// @brief Trim leading whitespace
    /// @param s input string
    /// @return left-stripped string
    std::string ltrim(const std::string &s);

    /// @brief Trim trailing whitespace
    /// @param s input string
    /// @return right-stripped string
    std::string rtrim(const std::string &s);

    /// @brief Remove consecutive whitespace in string
    /// @param s input string
    /// @return cleaned string
    std::string replaceWhitespaceWithSpace(const std::string &s);

    /// @brief Trim leading and trailing whitespace
    /// @param s input string
    /// @return stripped string
    std::string trim(const std::string &s);

    /// @brief Load the Discord Bot Token from File
    /// @param filename filename containing the bot token
    /// @param token reference used to set the output token string
    /// @return ErrorCode - whether the token was successfully read
    ErrorCode LoadToken(const std::string &filename, std::string &token);

    uint32_t RandomRoll();
    uint32_t RandomStar();

    std::string GenerateRandomCode(uint8_t length);

    ErrorCode LogActiveGuilds(const std::vector<std::string> &activeGuilds);

} // namespace Utils