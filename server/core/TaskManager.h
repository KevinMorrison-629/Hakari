#pragma once

#include <atomic>
#include <dpp/dpp.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <variant>
#include <vector>

#include "common/core/ThreadsafeQueue.h"
#include "server/core/CommandHandler.h"
#include "server/data/DataService.h"

namespace Core::Utils
{

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
        DPP_REACTION_ADD
    };
    using DiscordCommandParams = std::map<std::string, std::variant<std::string, int64_t, double>>;

    class Task
    {
    public:
        virtual ~Task() = default;
        virtual void process() const = 0;
        TaskPriority priority;
        TaskType type;
    };

    class TaskDiscordCommand : public Task
    {
    public:
        void process() const override
        {
            if (!command_handler || !data_service)
            {
                std::cerr << "Error: Task is missing required services." << std::endl;
                dpp::message response("An internal error has occurred. Please try again later.");
                if (bot_cluster && !interaction_token.empty())
                    bot_cluster->interaction_response_edit(interaction_token, response);
                return;
            }
            auto handler_opt = command_handler->get_handler(command_name);
            if (handler_opt)
            {
                (*handler_opt)(*this);
            }
            else
            {
                dpp::message response("This command is not yet implemented!");
                if (bot_cluster && !interaction_token.empty())
                    bot_cluster->interaction_response_edit(interaction_token, response);
            }
        }

    public:
        std::string interaction_token;
        std::string command_name;
        DiscordCommandParams parameters;
        dpp::snowflake user_id;
        std::shared_ptr<dpp::cluster> bot_cluster;
        std::shared_ptr<Core::Commands::CommandHandler> command_handler;
        std::shared_ptr<Core::Data::DataService> data_service;
    };

    class TaskManager
    {
    public:
        TaskManager(size_t num_threads) : m_done(false)
        {
            for (size_t i = 0; i < num_threads; ++i)
            {
                m_workers.emplace_back(&TaskManager::WorkerLoop, this);
            }
        }
        ~TaskManager()
        {
            m_done = true;
            for (auto &worker : m_workers)
            {
                if (worker.joinable())
                    worker.join();
            }
        }
        void submit(std::unique_ptr<Task> task)
        {
            if (!task)
                return;
            switch (task->priority)
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

    private:
        void WorkerLoop()
        {
            while (!m_done)
            {
                std::unique_ptr<Task> task;
                if (TryPopWeighted(task))
                {
                    task->process();
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        }
        bool TryPopWeighted(std::unique_ptr<Task> &task)
        {
            for (int i = 0; i < 5; ++i)
                if (m_highPriorityQueue.try_pop(task))
                    return true;
            for (int i = 0; i < 3; ++i)
                if (m_standardPriorityQueue.try_pop(task))
                    return true;
            if (m_lowPriorityQueue.try_pop(task))
                return true;
            return false;
        }
        ThreadsafeQueue<std::unique_ptr<Task>> m_highPriorityQueue;
        ThreadsafeQueue<std::unique_ptr<Task>> m_standardPriorityQueue;
        ThreadsafeQueue<std::unique_ptr<Task>> m_lowPriorityQueue;
        std::atomic<bool> m_done;
        std::vector<std::thread> m_workers;
    };
} // namespace Core::Utils