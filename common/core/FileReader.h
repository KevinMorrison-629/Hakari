#pragma once

#include <fstream>
#include <stdexcept> // Added for std::runtime_error
#include <string>
#include <vector> // Added for std::vector

namespace Core::Utils
{
    /// @brief Reads the entire content of a file into a single string.
    /// @param filepath The relative or absolute path to the file.
    /// @return A string containing the entire contents of the file.
    /// @throw std::runtime_error if the file cannot be opened.
    inline std::string ReadFile(const std::string &filepath)
    {
        std::ifstream inputFileStream(filepath);

        if (!inputFileStream.is_open())
        {
            throw std::runtime_error("Could not open file: " + filepath);
        }

        // Read the file into a string using stream iterators
        return std::string(std::istreambuf_iterator<char>(inputFileStream), std::istreambuf_iterator<char>());
    }

    /// @brief Reads a file into a vector of strings, one for each line.
    /// @param filepath The relative or absolute path to the file.
    /// @return A vector of strings, where each element is a line from the file.
    /// @throw std::runtime_error if the file cannot be opened.
    inline std::vector<std::string> ReadFileLines(const std::string &filepath)
    {
        std::ifstream inputFileStream(filepath);
        if (!inputFileStream.is_open())
        {
            throw std::runtime_error("Could not open file: " + filepath);
        }

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(inputFileStream, line))
        {
            // Optional: trim whitespace from the end of the line if your editor adds it
            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }
            lines.push_back(line);
        }

        return lines;
    }

} // namespace Core::Utils
