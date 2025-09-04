#include "WebService.h"
#include "nlohmann/json.hpp"
#include "server/tasks/Tasks.h"

using json = nlohmann::json;

namespace Core::Web
{

    // A new Task type for handling web requests asynchronously
    class TaskWebRequest : public Core::Utils::Task
    {
    public:
        void process() const override
        {
            // Placeholder for any complex web-related tasks.
            // For now, our routes are simple enough to be handled synchronously.
        }
        // Add any necessary data for the task
    };

    WebService::WebService(std::shared_ptr<Core::Utils::TaskManager> &taskManager,
                           std::shared_ptr<Core::Data::DataService> &dataService)
        : m_taskManager(taskManager), m_dataService(dataService)
    {
        m_httpServer = std::make_shared<QNET::HttpServer>();
    }

    void WebService::Initialize(uint16_t port)
    {
        setupRoutes();

        // Serve static files from the frontend build directory
        m_httpServer->ServeStaticFiles("/", "../frontend/build");

        std::cout << "HTTP Server initializing on http://localhost:" << port << std::endl;

        // Run the server in its own thread to not block the main application
        std::thread httpThread(&QNET::HttpServer::Run, m_httpServer, port);
        httpThread.detach();
    }

    void WebService::Run()
    {
        // The Run logic is handled by the detached thread in Initialize
    }

    void WebService::Stop()
    {
        if (m_httpServer)
        {
            m_httpServer->Stop();
        }
    }

    void WebService::setupRoutes()
    {
        m_httpServer->Post("/api/open_pack",
                           [this](const QNET::Request &req, QNET::Response &res)
                           {
                               json response_body;
                               try
                               {
                                   json request_data = json::parse(req.body);
                                   int64_t discord_id = request_data["discord_id"].get<int64_t>();

                                   // Use the existing DataService to find the player
                                   Core::Data::Player player =
                                       m_dataService->find_or_create_player_by_discord_id(discord_id);

                                   // Use the existing GameLogic to open a pack
                                   Core::Tasks::PackOpeningResult result =
                                       Core::Tasks::OpenPackForPlayer(*m_dataService, player);

                                   if (result.success)
                                   {
                                       response_body["success"] = true;
                                       response_body["message"] = result.message;

                                       json cards_array = json::array();
                                       for (size_t i = 0; i < result.opened_card_refs.size(); ++i)
                                       {
                                           json card_info;
                                           card_info["name"] = result.opened_card_refs[i].name;
                                           card_info["number"] = result.opened_card_objs[i].number;
                                           cards_array.push_back(card_info);
                                       }
                                       response_body["cards"] = cards_array;
                                       res.status = 200;
                                   }
                                   else
                                   {
                                       response_body["success"] = false;
                                       response_body["message"] = result.message;
                                       res.status = 400;
                                   }
                               }
                               catch (const json::parse_error &e)
                               {
                                   res.status = 400;
                                   response_body["success"] = false;
                                   response_body["message"] = "Invalid JSON format.";
                               }
                               catch (const std::exception &e)
                               {
                                   res.status = 500;
                                   response_body["success"] = false;
                                   response_body["message"] = "An internal server error occurred.";
                               }

                               res.set_content(response_body.dump(), "application/json");
                           });

        // Add login/registration routes here in the future
    }

} // namespace Core::Web
