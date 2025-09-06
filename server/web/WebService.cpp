#include "WebService.h"

#include "nlohmann/json.hpp"
#include <algorithm> // Required for std::sort
#include <filesystem>
#include <iostream>

#include "server/tasks/Tasks.h"

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

    bool WebService::checkFrontendDirectory(const std::string &frontendDir)
    {
        try
        {
            std::filesystem::path fs_path(frontendDir);
            std::cout << "[WebService] Attempting to serve static files from: " << fs_path.string() << std::endl;
            std::cout << "[WebService] Absolute path resolves to: " << std::filesystem::absolute(fs_path).string()
                      << std::endl;

            if (!std::filesystem::exists(fs_path))
            {
                std::cerr << "[WebService] WARNING: This directory does not exist!" << std::endl;
                return false;
            }
            else if (!std::filesystem::is_directory(fs_path))
            {
                std::cerr << "[WebService] WARNING: This path is a file, not a directory!" << std::endl;
                return false;
            }
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            std::cerr << "[WebService] Filesystem error: " << e.what() << std::endl;
            return false;
        }

        return true;
    }

    void WebService::Initialize(uint16_t port)
    {
        m_port = port;
        std::cout << "Initializing WebService on port " << m_port << "..." << std::endl;

        // Define the directory where the frontend files are located.
        // This path should be relative to the executable's location.
        const std::string frontend_directory = "./server/web/frontend";

        if (checkFrontendDirectory(frontend_directory))
        {
            // Set up all the API routes and static file serving.
            setupRoutes(frontend_directory);
        }
    }

    void WebService::Run()
    {
        if (!m_httpServer)
        {
            std::cerr << "HttpServer is not initialized. Cannot run." << std::endl;
            return;
        }
        std::cout << "WebService is running on port " << m_port << "." << std::endl;
        // This is a blocking call that starts the server.
        m_httpServer->Run(m_port);
    }

    void WebService::Stop()
    {
        if (m_httpServer)
        {
            std::cout << "Stopping WebService..." << std::endl;
            m_httpServer->Stop();
        }
    }

    // This is a lambda that wraps our route handlers. It checks for a valid JWT.
    auto requireAuth =
        [](const std::function<void(const QNET::Request &, QNET::Response &, const Core::Tasks::decoded_token &)> &handler)
    {
        return [handler](const QNET::Request &req, QNET::Response &res)
        {
            auto auth_header = req.get_header_value("Authorization");
            if (auth_header.rfind("Bearer ", 0) != 0)
            {
                res.status = 401;
                res.set_content("{\"success\": false, \"message\": \"Unauthorized: Missing or malformed token.\"}",
                                "application/json");
                return;
            }

            std::string token = auth_header.substr(7); // Remove "Bearer "
            auto decoded_jwt_opt = Core::Tasks::VerifyAndDecodeJwt(token);

            if (!decoded_jwt_opt)
            {
                res.status = 401;
                res.set_content("{\"success\": false, \"message\": \"Unauthorized: Invalid or expired token.\"}",
                                "application/json");
                return;
            }

            handler(req, res, *decoded_jwt_opt);
        };
    };

    void WebService::setupRoutes(const std::string &frontend_dir)
    {
        using json = nlohmann::json;

        if (!m_httpServer)
        {
            std::cerr << "HttpServer is not initialized. Cannot set up routes." << std::endl;
            return;
        }

        m_httpServer->Post("/api/register",
                           [this](const QNET::Request &req, QNET::Response &res)
                           {
                               json response_body;
                               try
                               {
                                   json request_data = json::parse(req.body);
                                   std::string email = request_data["email"].get<std::string>();
                                   std::string password = request_data["password"].get<std::string>();
                                   std::string displayName = request_data["displayName"];

                                   Core::Tasks::RegistrationResult result =
                                       Core::Tasks::RegisterUser(*m_dataService, email, password, displayName);

                                   response_body["success"] = result.success;
                                   response_body["message"] = result.message;
                                   res.status = result.success ? 201 : 400;
                               }
                               catch (const json::parse_error &e)
                               {
                                   res.status = 400;
                                   response_body["success"] = false;
                                   response_body["message"] = "Invalid JSON format: " + std::string(e.what());
                               }
                               catch (const std::exception &e)
                               {
                                   res.status = 500;
                                   response_body["success"] = false;
                                   response_body["message"] = "An internal server error occurred.";
                               }
                               res.set_content(response_body.dump(), "application/json");
                           });

        // NEW: Route for user login
        m_httpServer->Post("/api/login",
                           [this](const QNET::Request &req, QNET::Response &res)
                           {
                               json response_body;
                               try
                               {
                                   json request_data = json::parse(req.body);
                                   std::string email = request_data["email"].get<std::string>();
                                   std::string password = request_data["password"].get<std::string>();

                                   Core::Tasks::LoginResult result = Core::Tasks::LoginUser(*m_dataService, email, password);

                                   response_body["success"] = result.success;
                                   response_body["message"] = result.message;
                                   if (result.success)
                                   {
                                       response_body["token"] = result.token;
                                   }
                                   res.status = result.success ? 200 : 401;
                               }
                               catch (const json::parse_error &e)
                               {
                                   res.status = 400;
                                   response_body["success"] = false;
                                   response_body["message"] = "Invalid JSON format: " + std::string(e.what());
                               }
                               catch (const std::exception &e)
                               {
                                   res.status = 500;
                                   response_body["success"] = false;
                                   response_body["message"] = "An internal server error occurred.";
                               }
                               res.set_content(response_body.dump(), "application/json");
                           });

        m_httpServer->Post(
            "/api/open_pack",
            requireAuth(
                [this](const QNET::Request &req, QNET::Response &res, const Core::Tasks::decoded_token &decoded_token)
                {
                    json response_body;
                    try
                    {
                        // Get user ID securely from the token, NOT from the request body.
                        std::string player_id_str = decoded_token.get_payload_claim("user_id").as_string();
                        auto user_cards_query = QDB::Query().eq("_id", bsoncxx::oid(player_id_str));
                        auto player_opt = m_dataService->players.find_one(user_cards_query);

                        if (!player_opt)
                        {
                            res.status = 404;
                            response_body["success"] = false;
                            response_body["message"] = "Player not found.";
                            res.set_content(response_body.dump(), "application/json");
                            return;
                        }

                        Core::Tasks::PackOpeningResult result = Core::Tasks::OpenPackForPlayer(*m_dataService, *player_opt);

                        response_body["success"] = result.success;
                        response_body["message"] = result.message;
                        if (result.success)
                        {
                            json cards_array;
                            for (size_t i = 0; i < result.opened_card_refs.size(); ++i)
                            {
                                json card_info;
                                card_info["name"] = result.opened_card_refs[i].name;
                                card_info["number"] = result.opened_card_objs[i].number;
                                card_info["image"] = "https://hotpink-octopus-624350.hostingersite.com/character/" +
                                                     result.opened_card_refs[i].characterId.to_string();
                                cards_array.push_back(card_info);
                            }
                            response_body["cards"] = cards_array;
                            res.status = 200;
                        }
                        else
                        {
                            res.status = 400;
                        }
                    }
                    catch (const std::exception &e)
                    {
                        res.status = 500;
                        response_body["success"] = false;
                        response_body["message"] = "An internal server error occurred.";
                    }

                    res.set_content(response_body.dump(), "application/json");
                }));

        // REPLACES /api/inventory: Fetches inventory AND decks for the player.
        m_httpServer->Get(
            "/api/collection",
            requireAuth(
                [this](const QNET::Request &req, QNET::Response &res, const Core::Tasks::decoded_token &decoded_token)
                {
                    json response_body;
                    try
                    {
                        std::string user_id_str = decoded_token.get_payload_claim("user_id").as_string();
                        auto player_query = QDB::Query().eq("_id", bsoncxx::oid(user_id_str));
                        auto player_opt = m_dataService->players.find_one(player_query);

                        if (!player_opt)
                        {
                            res.status = 404;
                            response_body["success"] = false;
                            response_body["message"] = "Player not found.";
                            res.set_content(response_body.dump(), "application/json");
                            return;
                        }

                        // Ensure player has exactly 3 decks.
                        const int required_deck_count = 3;
                        if (player_opt->decks.size() < required_deck_count)
                        {
                            int decks_to_add = required_deck_count - player_opt->decks.size();
                            auto update = QDB::Update();
                            for (int i = 0; i < decks_to_add; ++i)
                            {
                                // Add to the update operation for the database
                                update.push("decks", std::vector<bsoncxx::oid>());

                                // Also add to the in-memory object to ensure the response is correct
                                player_opt->decks.push_back({});
                            }
                            m_dataService->players.update_one(player_query, update);
                        }

                        // 1. Fetch Inventory
                        auto user_cards_query = QDB::Query().in("_id", player_opt->inventory);
                        std::vector<Core::Data::CardObject> user_card_objects =
                            m_dataService->card_objects.find(user_cards_query);

                        json inventory_array = json::array();
                        for (const auto &card_obj : user_card_objects)
                        {
                            auto card_ref_opt =
                                m_dataService->card_references.find_one(QDB::Query().eq("_id", card_obj.cardReferenceId));
                            if (card_ref_opt)
                            {
                                json card_info;
                                card_info["id"] = card_obj.get_id_str(); // IMPORTANT: Send the CardObject ID
                                card_info["name"] = card_ref_opt->name;
                                card_info["number"] = card_obj.number;
                                card_info["image"] = "https://hotpink-octopus-624350.hostingersite.com/character/" +
                                                     card_ref_opt->characterId.to_string();
                                inventory_array.push_back(card_info);
                            }
                        }

                        // 2. Fetch Decks
                        json decks_array = json::array();
                        for (const auto &deck : player_opt->decks)
                        {
                            json deck_json = json::array();
                            for (const auto &card_oid : deck)
                            {
                                deck_json.push_back(card_oid.to_string());
                            }
                            decks_array.push_back(deck_json);
                        }

                        response_body["success"] = true;
                        response_body["inventory"] = inventory_array;
                        response_body["decks"] = decks_array;
                        res.status = 200;
                    }
                    catch (const std::exception &e)
                    {
                        res.status = 500;
                        response_body["success"] = false;
                        response_body["message"] = "An internal server error occurred while fetching collection.";
                    }
                    res.set_content(response_body.dump(), "application/json");
                }));

        // NEW: Update an existing deck by its index.
        m_httpServer->Put(
            "/api/decks",
            requireAuth(
                [this](const QNET::Request &req, QNET::Response &res, const Core::Tasks::decoded_token &decoded_token)
                {
                    json response_body;
                    try
                    {
                        json request_data = json::parse(req.body);

                        if (!request_data.contains("deckIndex"))
                        {
                            res.status = 400;
                            response_body["success"] = false;
                            response_body["message"] = "Request body must include 'deckIndex'.";
                            res.set_content(response_body.dump(), "application/json");
                            return;
                        }
                        int deckIndex = request_data["deckIndex"].get<int>();

                        std::vector<bsoncxx::oid> new_deck_oids;
                        for (const auto &item : request_data["cards"])
                        {
                            if (item.is_string())
                            {
                                new_deck_oids.emplace_back(bsoncxx::oid(item.get<std::string>()));
                            }
                        }

                        std::string user_id_str = decoded_token.get_payload_claim("user_id").as_string();
                        auto filter = QDB::Query().eq("_id", bsoncxx::oid(user_id_str));

                        // First, verify the player exists and the deck index is valid.
                        auto player_opt = m_dataService->players.find_one(filter);
                        if (!player_opt)
                        {
                            res.status = 404;
                            response_body["success"] = false;
                            response_body["message"] = "Could not save deck. Player not found.";
                            res.set_content(response_body.dump(), "application/json");
                            return;
                        }
                        if (deckIndex < 0 || deckIndex >= player_opt->decks.size())
                        {
                            res.status = 400;
                            response_body["success"] = false;
                            response_body["message"] = "Could not save deck. Invalid deck index.";
                            res.set_content(response_body.dump(), "application/json");
                            return;
                        }

                        // --- NEW LOGIC: Pre-check if the deck has changed ---
                        auto &original_deck_oids = player_opt->decks[deckIndex];

                        // To perform an order-agnostic comparison, we sort copies of both vectors.
                        auto sorted_original = original_deck_oids;
                        std::sort(sorted_original.begin(), sorted_original.end());

                        auto sorted_new = new_deck_oids;
                        std::sort(sorted_new.begin(), sorted_new.end());

                        if (sorted_original == sorted_new)
                        {
                            // The decks are identical, no need to update the database.
                            res.status = 200;
                            response_body["success"] = true;
                            response_body["message"] = "Deck saved successfully (no changes detected).";
                            res.set_content(response_body.dump(), "application/json");
                            return;
                        }
                        // --- END NEW LOGIC ---

                        // If we reach here, the decks are different, so proceed with the update.
                        auto update = QDB::Update().set("decks." + std::to_string(deckIndex), new_deck_oids);
                        m_dataService->players.update_one(filter, update);

                        res.status = 200;
                        response_body["success"] = true;
                        response_body["message"] = "Deck saved successfully.";
                    }
                    catch (const std::exception &e)
                    {
                        res.status = 500;
                        response_body["success"] = false;
                        response_body["message"] = "Server error while saving deck.";
                    }
                    res.set_content(response_body.dump(), "application/json");
                }));

        // --- Static File Serving ---

        // Serve all files from the specified directory at the root URL ("/").
        // This must be set up AFTER API routes to ensure API calls are not treated as file requests.
        bool success = m_httpServer->ServeStaticFiles("/", frontend_dir);
        if (!success)
        {
            std::cerr << "Failed to serve static files from directory: " << frontend_dir << std::endl;
            std::cerr << "Please ensure the directory exists and the executable has permission to read it." << std::endl;
        }
        else
        {
            std::cout << "Serving static files from: " << frontend_dir << std::endl;
        }
    }

} // namespace Core::Web
