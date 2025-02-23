#pragma once

#include <iostream>
#include <string>

namespace Utils
{
    namespace Logger
    {
        // Setup Constants for Log Types
        enum class Type : uint8_t
        {
            DEBUG = 0,
            INFO = 1,
            WARNING = 2,
            FATAL_ERROR = 3
        };

        static void Log(Type type, const std::string &message)
        {
            std::string typestring = "UNKNOWN";
            if (type == Type::DEBUG)
                typestring = "Debug";
            else if (type == Type::INFO)
                typestring = "Info";
            else if (type == Type::WARNING)
                typestring = "Warning";
            else if (type == Type::FATAL_ERROR)
                typestring = "Error";

            std::cout << "[" << typestring << "]: " << message << std::endl;
        }

    } // namespace Logger

} // namespace Utils