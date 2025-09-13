#include "WebService.h"

#include "nlohmann/json.hpp"
#include <algorithm> // Required for std::sort
#include <filesystem>
#include <iostream>
#include <set> // Used to find unique OIDs

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

        const std::string frontend_directory = "./server/web/frontend";

        if (checkFrontendDirectory(frontend_directory))
        {
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
            std::string token = auth_header.substr(7);
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

        // --- AUTH ROUTES ---
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
                                   response_body["message"] =
                                       "An internal server error occurred. {" + std::string(e.what()) + "}";
                               }
                               res.set_content(response_body.dump(), "application/json");
                           });
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
                                   response_body["message"] =
                                       "An internal server error occurred. {" + std::string(e.what()) + "}";
                               }
                               res.set_content(response_body.dump(), "application/json");
                           });

        // --- START FRIEND SYSTEM ROUTES ---

        m_httpServer->Get(
            "/api/users/search",
            requireAuth(
                [this](const QNET::Request &req, QNET::Response &res, const Core::Tasks::decoded_token &decoded_token)
                {
                    json response_body;
                    try
                    {
                        auto name_param = req.get_param_value("name");
                        if (name_param.empty())
                        {
                            res.status = 400;
                            response_body = {{"success", false}, {"message", "Search parameter 'name' is required."}};
                            res.set_content(response_body.dump(), "application/json");
                            return;
                        }

                        std::string current_user_id_str = decoded_token.get_payload_claim("user_id").as_string();
                        bsoncxx::oid current_user_oid(current_user_id_str);

                        auto current_player_opt = m_dataService->players.find_one(QDB::Query().eq("_id", current_user_oid));
                        if (!current_player_opt)
                        {
                            res.status = 404;
                            response_body = {{"success", false}, {"message", "Current player not found."}};
                            res.set_content(response_body.dump(), "application/json");
                            return;
                        }

                        auto filter =
                            QDB::Query().regex("displayName", std::string(name_param), "i").ne("_id", current_user_oid);

                        auto results = m_dataService->players.find(filter);

                        json users_array = json::array();
                        for (const auto &player : results)
                        {
                            json user_info;
                            user_info["_id"] = player.get_id_str();
                            user_info["displayName"] = player.display_name;

                            // Determine user status relative to the current player
                            if (std::find(current_player_opt->friends.begin(), current_player_opt->friends.end(),
                                          player.get_id()) != current_player_opt->friends.end())
                            {
                                user_info["status"] = "friend";
                            }
                            else if (std::find(current_player_opt->friend_requests_sent.begin(),
                                               current_player_opt->friend_requests_sent.end(),
                                               player.get_id()) != current_player_opt->friend_requests_sent.end())
                            {
                                user_info["status"] = "pending";
                            }
                            else
                            {
                                user_info["status"] = "none";
                            }
                            users_array.push_back(user_info);
                        }

                        response_body["success"] = true;
                        response_body["users"] = users_array;
                        res.status = 200;
                    }
                    catch (const std::exception &e)
                    {
                        res.status = 500;
                        response_body = {{"success", false},
                                         {"message", "An internal server error occurred during user search."}};
                    }
                    res.set_content(response_body.dump(), "application/json");
                }));

        m_httpServer->Get(
            "/api/friends",
            requireAuth(
                [this](const QNET::Request &req, QNET::Response &res, const Core::Tasks::decoded_token &decoded_token)
                {
                    json response_body;
                    try
                    {
                        std::string user_id_str = decoded_token.get_payload_claim("user_id").as_string();
                        auto player_opt = m_dataService->players.find_one(QDB::Query().eq("_id", bsoncxx::oid(user_id_str)));

                        if (!player_opt)
                        {
                            res.status = 404;
                            response_body = {{"success", false}, {"message", "Player not found."}};
                            res.set_content(response_body.dump(), "application/json");
                            return;
                        }

                        std::set<bsoncxx::oid> user_ids_to_find;
                        user_ids_to_find.insert(player_opt->friends.begin(), player_opt->friends.end());
                        user_ids_to_find.insert(player_opt->friend_requests_received.begin(),
                                                player_opt->friend_requests_received.end());
                        user_ids_to_find.insert(player_opt->friend_requests_sent.begin(),
                                                player_opt->friend_requests_sent.end());

                        std::map<bsoncxx::oid, Core::Data::Player> player_map;
                        if (!user_ids_to_find.empty())
                        {
                            std::vector<bsoncxx::oid> oids_vec(user_ids_to_find.begin(), user_ids_to_find.end());
                            auto players_data = m_dataService->players.find(QDB::Query().in("_id", oids_vec));
                            for (const auto &p : players_data)
                                player_map[p.get_id()] = p;
                        }

                        auto populate_user_array = [&](const std::vector<bsoncxx::oid> &ids)
                        {
                            json arr = json::array();
                            for (const auto &id : ids)
                            {
                                if (player_map.count(id))
                                {
                                    arr.push_back({{"_id", player_map[id].get_id_str()},
                                                   {"displayName", player_map[id].display_name}});
                                }
                            }
                            return arr;
                        };

                        response_body["friends"] = populate_user_array(player_opt->friends);
                        response_body["incomingRequests"] = populate_user_array(player_opt->friend_requests_received);
                        response_body["outgoingRequests"] = populate_user_array(player_opt->friend_requests_sent);
                        response_body["success"] = true;
                        res.status = 200;
                    }
                    catch (const std::exception &e)
                    {
                        res.status = 500;
                        response_body = {{"success", false},
                                         {"message", "An internal server error occurred while fetching friends data."}};
                    }
                    res.set_content(response_body.dump(), "application/json");
                }));

        m_httpServer->Post(
            "/api/friends/request",
            requireAuth(
                [this](const QNET::Request &req, QNET::Response &res, const Core::Tasks::decoded_token &decoded_token)
                {
                    json response_body;
                    try
                    {
                        json request_data = json::parse(req.body);
                        std::string sender_id_str = decoded_token.get_payload_claim("user_id").as_string();
                        std::string recipient_id_str = request_data["recipientId"];
                        bsoncxx::oid sender_oid(sender_id_str);
                        bsoncxx::oid recipient_oid(recipient_id_str);

                        auto sender_player = m_dataService->players.find_one(QDB::Query().eq("_id", sender_oid));
                        if (sender_player)
                        {
                            if (std::find(sender_player->friends.begin(), sender_player->friends.end(), recipient_oid) !=
                                sender_player->friends.end())
                            {
                                res.status = 400;
                                response_body = {{"success", false}, {"message", "You are already friends with this user."}};
                                res.set_content(response_body.dump(), "application/json");
                                return;
                            }
                        }

                        m_dataService->players.update_one(QDB::Query().eq("_id", sender_oid),
                                                          QDB::Update().add_to_set("friendRequestsSent", recipient_oid));
                        m_dataService->players.update_one(QDB::Query().eq("_id", recipient_oid),
                                                          QDB::Update().add_to_set("friendRequestsReceived", sender_oid));

                        response_body = {{"success", true}, {"message", "Friend request sent."}};
                        res.status = 200;
                    }
                    catch (const std::exception &e)
                    {
                        res.status = 500;
                        response_body = {{"success", false}, {"message", "Server error sending friend request."}};
                    }
                    res.set_content(response_body.dump(), "application/json");
                }));

        m_httpServer->Post(
            "/api/friends/response",
            requireAuth(
                [this](const QNET::Request &req, QNET::Response &res, const Core::Tasks::decoded_token &decoded_token)
                {
                    json response_body;
                    try
                    {
                        json request_data = json::parse(req.body);
                        std::string current_user_id_str = decoded_token.get_payload_claim("user_id").as_string();
                        std::string other_user_id_str = request_data["otherUserId"];
                        std::string action = request_data["action"];
                        bsoncxx::oid current_user_oid(current_user_id_str);
                        bsoncxx::oid other_user_oid(other_user_id_str);

                        if (action == "accept")
                        {
                            // Add to each other's friends lists
                            m_dataService->players.update_one(QDB::Query().eq("_id", current_user_oid),
                                                              QDB::Update().add_to_set("friends", other_user_oid));
                            m_dataService->players.update_one(QDB::Query().eq("_id", other_user_oid),
                                                              QDB::Update().add_to_set("friends", current_user_oid));
                            response_body["message"] = "Friend request accepted.";
                        }
                        else
                        { // Decline or Cancel
                            response_body["message"] =
                                action == "decline" ? "Friend request declined." : "Friend request cancelled.";
                        }

                        // Clean up ALL pending requests between the two users, regardless of action
                        m_dataService->players.update_one(QDB::Query().eq("_id", current_user_oid),
                                                          QDB::Update()
                                                              .pull("friendRequestsReceived", other_user_oid)
                                                              .pull("friendRequestsSent", other_user_oid));
                        m_dataService->players.update_one(QDB::Query().eq("_id", other_user_oid),
                                                          QDB::Update()
                                                              .pull("friendRequestsSent", current_user_oid)
                                                              .pull("friendRequestsReceived", current_user_oid));

                        response_body["success"] = true;
                        res.status = 200;
                    }
                    catch (const std::exception &e)
                    {
                        res.status = 500;
                        response_body = {{"success", false}, {"message", "Server error responding to friend request."}};
                    }
                    res.set_content(response_body.dump(), "application/json");
                }));

        m_httpServer->Delete(
            "/api/friends/:friendId",
            requireAuth(
                [this](const QNET::Request &req, QNET::Response &res, const Core::Tasks::decoded_token &decoded_token)
                {
                    json response_body;
                    try
                    {
                        std::string current_user_id_str = decoded_token.get_payload_claim("user_id").as_string();
                        std::string friend_id_str = req.matches[1].str();
                        bsoncxx::oid current_user_oid(current_user_id_str);
                        bsoncxx::oid friend_oid(friend_id_str);

                        m_dataService->players.update_one(QDB::Query().eq("_id", current_user_oid),
                                                          QDB::Update().pull("friends", friend_oid));
                        m_dataService->players.update_one(QDB::Query().eq("_id", friend_oid),
                                                          QDB::Update().pull("friends", current_user_oid));

                        response_body = {{"success", true}, {"message", "Friend removed."}};
                        res.status = 200;
                    }
                    catch (const std::exception &e)
                    {
                        res.status = 500;
                        response_body = {{"success", false}, {"message", "Server error removing friend."}};
                    }
                    res.set_content(response_body.dump(), "application/json");
                }));

        // --- END FRIEND SYSTEM ROUTES ---

        // --- OTHER API ROUTES (PACK OPENING, COLLECTION, DECKS) ---
        m_httpServer->Post(
            "/api/open_pack",
            requireAuth(
                [this](const QNET::Request &req, QNET::Response &res, const Core::Tasks::decoded_token &decoded_token)
                {
                    json response_body;
                    try
                    {
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

        m_httpServer->Get(
            "/api/collection/:userId",
            requireAuth(
                [this](const QNET::Request &req, QNET::Response &res, const Core::Tasks::decoded_token &decoded_token)
                {
                    json response_body;
                    try
                    {
                        std::string requested_user_id_param = req.matches[1].str();
                        std::string current_user_id_str = decoded_token.get_payload_claim("user_id").as_string();
                        std::string target_user_id_str = requested_user_id_param;

                        if (requested_user_id_param == "@me")
                        {
                            target_user_id_str = current_user_id_str;
                        }

                        bool is_owner = (target_user_id_str == current_user_id_str);

                        auto player_query = QDB::Query().eq("_id", bsoncxx::oid(target_user_id_str));
                        auto player_opt = m_dataService->players.find_one(player_query);

                        if (!player_opt)
                        {
                            res.status = 404;
                            response_body["success"] = false;
                            response_body["message"] = "Player not found.";
                            res.set_content(response_body.dump(), "application/json");
                            return;
                        }

                        // --- Always fetch inventory ---
                        auto user_cards_query = QDB::Query().in("_id", player_opt->cards);
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
                                card_info["id"] = card_obj.get_id_str();
                                card_info["name"] = card_ref_opt->name;
                                card_info["number"] = card_obj.number;
                                card_info["image"] = "https://hotpink-octopus-624350.hostingersite.com/character/" +
                                                     card_ref_opt->characterId.to_string();
                                card_info["attackPoints"] = card_obj.attackPoints;
                                card_info["healthPoints"] = card_obj.healthPoints;
                                card_info["tier"] = Core::Data::CardTier_to_String(card_ref_opt->tier);

                                std::string ability_text = "";
                                if (card_ref_opt && card_ref_opt->abilityId.size() > 0)
                                {
                                    auto ability_ref_opt = m_dataService->ability_references.find_one(
                                        QDB::Query().eq("_id", card_ref_opt->abilityId));
                                    if (ability_ref_opt)
                                    {
                                        ability_text = ability_ref_opt->name;
                                    }
                                }
                                card_info["ability"] = ability_text;

                                inventory_array.push_back(card_info);
                            }
                        }
                        response_body["inventory"] = inventory_array;

                        // --- Conditionally fetch decks for owner ---
                        if (is_owner)
                        {
                            const int required_deck_count = 3;
                            if (player_opt->decks.size() < required_deck_count)
                            {
                                int decks_to_add = required_deck_count - player_opt->decks.size();
                                auto update = QDB::Update();
                                for (int i = 0; i < decks_to_add; ++i)
                                {
                                    update.push("decks", std::vector<bsoncxx::oid>());
                                    player_opt->decks.push_back({});
                                }
                                m_dataService->players.update_one(player_query, update);
                            }

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
                            response_body["decks"] = decks_array;
                        }

                        response_body["success"] = true;
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

                        auto &original_deck_oids = player_opt->decks[deckIndex];
                        auto sorted_original = original_deck_oids;
                        std::sort(sorted_original.begin(), sorted_original.end());
                        auto sorted_new = new_deck_oids;
                        std::sort(sorted_new.begin(), sorted_new.end());

                        if (sorted_original == sorted_new)
                        {
                            res.status = 200;
                            response_body["success"] = true;
                            response_body["message"] = "Deck saved successfully (no changes detected).";
                            res.set_content(response_body.dump(), "application/json");
                            return;
                        }

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
        bool success = m_httpServer->ServeStaticFiles("/", frontend_dir);
        if (!success)
        {
            std::cerr << "Failed to serve static files from directory: " << frontend_dir << std::endl;
        }
        else
        {
            std::cout << "Serving static files from: " << frontend_dir << std::endl;
        }
    }

} // namespace Core::Web
