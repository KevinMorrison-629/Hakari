#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>

// Forward-declare TaskDiscordCommand to avoid a circular dependency.
// The full definition of the task is not needed in this header file.
namespace Core::Utils
{
    class TaskDiscordCommand;
}

namespace Core::Commands
{
    // A type alias for the function signature of all command handlers.
    using CommandHandlerFunc = std::function<void(const Core::Utils::TaskDiscordCommand &)>;

    /**
     * @brief A registry for slash command handlers. This class is responsible
     * for mapping command names to their corresponding handler functions.
     */
    class CommandHandler
    {
    public:
        /**
         * @brief Registers a function to handle a specific slash command.
         * @param command_name The name of the command (e.g., "drop").
         * @param handler The function that will be executed.
         */
        void register_command(const std::string &command_name, CommandHandlerFunc handler)
        {
            m_handlers[command_name] = handler;
        }

        /**
         * @brief Retrieves the handler function for a given command name.
         * @param command_name The name of the command to find.
         * @return An optional containing the handler function if found, otherwise std::nullopt.
         */
        std::optional<CommandHandlerFunc> get_handler(const std::string &command_name) const
        {
            auto it = m_handlers.find(command_name);
            if (it != m_handlers.end())
            {
                return it->second;
            }
            return std::nullopt;
        }

    private:
        std::map<std::string, CommandHandlerFunc> m_handlers;
    };

} // namespace Core::Commands