#pragma once

#include <mutex> // Note: mutex is included but not used in this file.
#include <random>
#include <string>

/// @brief Namespace for core utility functions and classes.
namespace Core::Utils
{
    /// @brief String containing all valid characters for random code generation.
    static const std::string VALID_CODE_CHARACTERS = "abcdefghijklmnopqrstuvwxyz1234567890";
    /// @brief Standard random device used for seeding the random number generator.
    static std::random_device rd;
    /// @brief Mersenne Twister 19937 random number generator, seeded by `rd`.
    inline static std::mt19937 gen(rd());
    /// @brief Uniform integer distribution for selecting characters from `VALID_CODE_CHARACTERS`.
    /// Note: The distribution range should ideally be `VALID_CODE_CHARACTERS.size() - 1` if used as an index.
    /// Using `VALID_CODE_CHARACTERS.size()` might lead to an out-of-bounds access if the upper bound is inclusive.
    /// However, std::uniform_int_distribution is [a, b], so size()-1 is correct.
    inline static std::uniform_int_distribution<uint64_t> random_code_dist(0, VALID_CODE_CHARACTERS.size() - 1);

    /// @brief Generates a random alphanumeric code of a specified length.
    /// The code consists of lowercase English letters and digits.
    /// @param length The desired length of the random code.
    /// @return A string representing the generated random code.
    inline std::string GenerateRandomCode(size_t length)
    {
        std::string code;
        code.reserve(length); // Pre-allocate memory for efficiency
        for (size_t i = 0; i < length; i++)
        {
            code += VALID_CODE_CHARACTERS[random_code_dist(gen)];
        }
        return code;
    }

    // TODO: Consider adding a proper UUID generation class/function if this namespace is intended for UUIDs.
    // For example, using a library or implementing RFC 4122.
}