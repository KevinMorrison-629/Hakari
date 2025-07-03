#pragma once

#include <iostream>
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

    // Define a generic task structure
    struct Task
    {
        TaskPriority priority;
        std::string name;
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
        void submit(Task task)
        {
            switch (task.priority)
            {
            case TaskPriority::High:
                m_highPriorityQueue.push(task);
                break;
            case TaskPriority::Standard:
                m_standardPriorityQueue.push(task);
                break;
            case TaskPriority::Low:
                m_lowPRiorityQueue.push(task);
                break;
            }
        }

    private:
        /// @brief The main loop for each worker thread.
        /// Implements the 5:3:1 weighted polling logic.
        void WorkerLoop()
        {
            Task task;
            while (!m_done)
            {
                // Attempt to process tasks based on weighted priority
                if (TryPopWeighted(task))
                {
                    ProcessTask(task);
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
        bool TryPopWeighted(Task &task)
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

        /// @brief Processes Task (stubbed out for now - just prints out task name)
        /// @param task task to process
        void ProcessTask(const Task &task)
        {
            std::cout << "Processing task '" << task.name << "' on thread "
                      << std::this_thread::get_id() << std::endl;
            // Simulate work
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

    private:
        // Queues for different priority levels
        ThreadsafeQueue<Task> m_highPriorityQueue;
        ThreadsafeQueue<Task> m_standardPriorityQueue;
        ThreadsafeQueue<Task> m_lowPRiorityQueue;

        std::atomic<bool> m_done;           // Flag to signal threads to shut down
        std::vector<std::thread> m_workers; // The thread pool
    };
}