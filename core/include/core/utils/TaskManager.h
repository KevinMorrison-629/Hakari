#pragma once

#include <dpp/dpp.h>
#include <iostream>
#include <memory>
#include <string>

#include "core/utils/ThreadsafeQueue.h"

namespace Core::Utils
{
    // Define the priority levels
    enum class TaskPriority
    {
        Low,
        Standard,
        High
    };

    enum class TaskType
    {
        MESSAGE,
        DPP_SLASH_COMMAND,
        DPP_REACTION_ADD,
    };

    // Define a generic task structure
    struct Task
    {
        TaskPriority priority;
        TaskType type;

        virtual ~Task() = default;

        virtual void process() const = 0;
    };

    struct TaskMessage : public Task
    {
        std::string message;

        void process() const override
        {
            std::cout << "Processing task '" << int(priority) << "' on thread " << std::this_thread::get_id() << std::endl;

            std::cout << "Message: " << message << std::endl;
        }
    };

    struct TaskDiscordCommand : public Task
    {
        std::string interaction_token;
        std::string command_name;
        std::map<std::string, std::variant<std::string, int64_t, double>> parameters;
        dpp::snowflake guild_id;
        dpp::snowflake user_id;
        std::shared_ptr<dpp::cluster> bot_cluster;

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
        /// @param task The task to be processed.
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
                    m_lowPRiorityQueue.push(std::move(task));
                    break;
                }
            }
        }

    private:
        /// @brief The main loop for each worker thread.
        /// Implements the 5:3:1 weighted polling logic.
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

        /// @brief Tries to pop from queues with 5:3:1 weighting.
        /// @param[out] task The task that was popped.
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
            if (m_lowPRiorityQueue.try_pop(task))
                return true;

            return false;
        }

    private:
        // Queues for different priority levels
        ThreadsafeQueue<std::unique_ptr<Task>> m_highPriorityQueue;
        ThreadsafeQueue<std::unique_ptr<Task>> m_standardPriorityQueue;
        ThreadsafeQueue<std::unique_ptr<Task>> m_lowPRiorityQueue;

        std::atomic<bool> m_done;           // Flag to signal threads to shut down
        std::vector<std::thread> m_workers; // The thread pool
    };
} // namespace Core::Utils