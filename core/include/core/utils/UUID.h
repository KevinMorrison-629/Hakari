#pragma once

#include <mutex>
#include <random>
#include <string>

namespace Core::Utils
{
    static const std::string VALID_CODE_CHARACTERS = "abcdefghijklmnopqrstuvwxyz1234567890";
    static std::random_device rd;
    inline static std::mt19937 gen(rd());
    inline static std::uniform_int_distribution<uint64_t> random_code_dist(0, VALID_CODE_CHARACTERS.size());

    inline std::string GenerateRandomCode(size_t length)
    {
        std::string code;
        for (size_t i = 0; i < length; i++)
        {
            code += VALID_CODE_CHARACTERS[random_code_dist(gen)];
        }
        return code;
    }
}