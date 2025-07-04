#pragma once

#include <string>
#include <fstream>

namespace Core::Utils
{
    std::string ReadFile(const std::string &filepath)
    {
        std::ifstream inputFileStream(filepath);

        if (!inputFileStream.is_open())
        {
            throw std::runtime_error("Could not open file: " + filepath);
        }

        return std::string(std::istreambuf_iterator<char>(inputFileStream),
                           std::istreambuf_iterator<char>());
    }
}