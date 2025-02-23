#include "Utils/Utils.h"

#include <fstream>
#include <regex>

namespace Utils
{
    std::string ltrim(const std::string &s)
    {
        size_t start = s.find_first_not_of(" \t\n\r\f\v");
        return (start == std::string::npos) ? "" : s.substr(start);
    }

    std::string rtrim(const std::string &s)
    {
        size_t end = s.find_last_not_of(" \t\n\r\f\v");
        return (end == std::string::npos) ? "" : s.substr(0, end + 1);
    }

    std::string replaceWhitespaceWithSpace(const std::string &s)
    {
        // regex expression to match consecutive whitespace characters
        std::regex pattern("\\s+");

        // replace all occurences of consecutive whitespace with a single space
        return std::regex_replace(s, pattern, " ");
    }

    std::string trim(const std::string &s) { return replaceWhitespaceWithSpace(rtrim(ltrim(s))); }

    ErrorCode LoadToken(const std::string &filename, std::string &token)
    {
        std::ifstream file;
        file.open(filename);

        if (!file.is_open())
        {
            return ErrorCode::FILE_NOT_FOUND;
        }
        else
        {
            token = {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

            file.close();
        }

        return ErrorCode::SUCCESS;
    }

    std::string GenerateRandomCode(uint8_t length)
    {
        std::string code;
        for (uint32_t i = 0; i < length; i++)
        {
            code += VALID_CODE_CHARACTERS[random_code_dist(gen)];
        }
        return code;
    }

    ErrorCode LogActiveGuilds(const std::vector<std::string> &activeGuilds)
    {
        std::ofstream file;
        file.open("ActiveGuilds.txt");

        if (!file.is_open())
        {
            return ErrorCode::FILE_NOT_FOUND;
        }
        else
        {
            for (auto guild : activeGuilds)
            {
                file << guild << "\n";
            }
            file.close();
        }

        return ErrorCode::SUCCESS;
    }
} // namespace Utils