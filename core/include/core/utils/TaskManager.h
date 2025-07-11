#pragma once

#include <dpp/dpp.h>
#include <iostream>
#include <memory>
#include <string>

#include "core/utils/ThreadsafeQueue.h"

namespace Core::Utils
{
    /// @brief Defines the priority levels for tasks.
    enum class TaskPriority
    {
        Low,      ///< Low priority task.
        Standard, ///< Standard priority task.
        High      ///< High priority task.
    };

    /// @brief Defines the specific type of a task.
    enum class TaskType
    {
        MESSAGE,           ///< A simple message task.
        DPP_SLASH_COMMAND, ///< A Discord slash command task from DPP.
        DPP_REACTION_ADD,  ///< A Discord reaction add event task from DPP.
    };

    /// @brief Abstraction over the Command Parameters - a map containing variant parameters indexed by param name
    using DiscordCommandParams = std::map<std::string, std::variant<std::string, int64_t, double>>;

    /// @brief An abstract base class for a generic task.
    /// @details All specific task types must inherit from this class and implement the process method.
    class Task
    {
    public:
        /// @brief Virtual destructor to ensure proper cleanup of derived types.
        virtual ~Task() = default;

        /// @brief Pure virtual function to process the task. Must be overridden by derived classes.
        virtual void process() const = 0;

    public:
        TaskPriority priority; ///< @brief The priority level of the task.
        TaskType type;         ///< @brief The specific type of the task.
    };

    /// @brief A task for processing a simple string message. (used as a test message)
    /// @todo Remove task definition (should be unused in final release)
    class TaskMessage : public Task
    {
    public:
        /// @brief Processes the message task by printing it to the console.
        void process() const override
        {
            std::cout << "Processing task '" << int(priority) << "' on thread " << std::this_thread::get_id() << std::endl;
            std::cout << "Message: " << message << std::endl;
        }

    public:
        std::string message; ///< @brief The message content to be processed.
    };

    /// @brief A task for processing a Discord slash command.
    class TaskDiscordCommand : public Task
    {
        /// @brief Processes the slash command.
        void process() const override
        {
            std::cout << "Processing task '" << int(priority) << "' on thread " << std::this_thread::get_id() << std::endl;

            if (command_name == "ping")
            {
                dpp::message response;
                response.set_content("Pong!");
                bot_cluster->interaction_response_edit(interaction_token, response);
            }
        }

    public:
        std::string interaction_token;             ///< @brief The interaction token for responding to the command.
        std::string command_name;                  ///< @brief The name of the command that was invoked.
        DiscordCommandParams parameters;           ///< @brief A map of parameters provided with the command.
        dpp::snowflake guild_id;                   ///< @brief The ID of the guild where the command was used.
        dpp::snowflake user_id;                    ///< @brief The ID of the user who invoked the command.
        std::shared_ptr<dpp::cluster> bot_cluster; ///< @brief A shared pointer to the bot cluster to send responses.
    };

    /// @brief Manages a pool of threads that process tasks from three priority
    /// queues using a weighted round-robin (5:3:1) polling strategy.
    class TaskManager
    {
    public:
        /// @brief Constructs the TaskManager and starts the worker threads.
        /// @param num_threads The number of worker threads in the pool.
        TaskManager(size_t num_threads) : m_done(false)
        {
            // Create and launch the specified number of worker threads
            for (size_t i = 0; i < num_threads; ++i)
            {
                m_workers.emplace_back(&TaskManager::WorkerLoop, this);
            }
        }

        /// @brief Destructor that signals threads to stop and joins them.
        ~TaskManager()
        {
            // Signal all threads to finish their work and exit
            m_done = true;
            // Wait for all threads to complete
            for (auto &worker : m_workers)
            {
                if (worker.joinable())
                {
                    worker.join();
                }
            }
        }

        /// @brief Submits a new task to the appropriate queue.
        /// @param task A unique_ptr to the task to be processed.
        void submit(std::unique_ptr<Task> task)
        {
            if (task)
            {
                TaskPriority priority = task->priority;
                switch (priority)
                {
                case TaskPriority::High:
                    m_highPriorityQueue.push(std::move(task));
                    break;
                case TaskPriority::Standard:
                    m_standardPriorityQueue.push(std::move(task));
                    break;
                case TaskPriority::Low:
                    m_lowPriorityQueue.push(std::move(task));
                    break;
                }
            }
        }

    private:
        /// @brief The main loop for each worker thread.
        /// @details Fetches tasks from the queues using a weighted (5-3-1) polling
        /// strategy and processes them. If no tasks are available, it sleeps briefly
        /// to prevent busy-waiting.
        void WorkerLoop()
        {
            while (!m_done)
            {
                std::unique_ptr<Task> task;
                // Attempt to process tasks based on weighted priority
                if (TryPopWeighted(task))
                {
                    task->process();
                }
                else
                {
                    // If no tasks are available in any queue, sleep briefly
                    // to prevent the CPU from spinning needlessly (a "spin-lock").
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        }

        /// @brief Tries to pop a task from the queues with a 5:3:1 weighting.
        /// @param[out] task A reference to a unique_ptr where the popped task will be stored.
        /// @return true if a task was successfully popped, false otherwise.
        bool TryPopWeighted(std::unique_ptr<Task> &task)
        {
            // HIGH priority: try up to 5 times (weight 5)
            for (int i = 0; i < 5; ++i)
            {
                if (m_highPriorityQueue.try_pop(task))
                    return true;
            }

            // STANDARD priority: try up to 3 times (weight 3)
            for (int i = 0; i < 3; ++i)
            {
                if (m_standardPriorityQueue.try_pop(task))
                    return true;
            }

            // LOW priority: try 1 time (weight 1)
            if (m_lowPriorityQueue.try_pop(task))
                return true;

            return false;
        }

    private:
        // Queues for different priority levels
        ThreadsafeQueue<std::unique_ptr<Task>> m_highPriorityQueue;     ///< @brief Queue for high priority tasks.
        ThreadsafeQueue<std::unique_ptr<Task>> m_standardPriorityQueue; ///< @brief Queue for standard priority tasks.
        ThreadsafeQueue<std::unique_ptr<Task>> m_lowPriorityQueue;      ///< @brief Queue for low priority tasks.

        std::atomic<bool> m_done;           ///< @brief Atomic flag to signal worker threads to shut down.
        std::vector<std::thread> m_workers; ///< @brief The pool of worker threads.
    };
} // namespace Core::Utils