#pragma once

#include "server/data/DataService.h"

#include "jwt-cpp/jwt.h" // Assumes jwt-cpp is in your include path
#include <memory>
#include <sodium.h>
#include <string>
#include <vector>

namespace Core::Tasks
{
    // --- Result Structs ---
    struct RegistrationResult
    {
        bool success = false;
        std::string message;
    };

    struct LoginResult
    {
        bool success = false;
        std::string message;
        std::string token;
    };

    // --- Helper Function ---
    inline std::string CreateJwtForPlayer(const Core::Data::Player &player)
    {
        // You MUST change this to a long, random, secret string in a config file
        const std::string JWT_SECRET = "a1LS3rL1bN45KWKE7URaSbRZxY8QaAqim2P6wzZ";

        auto token = jwt::create()
                         .set_issuer("CardForge")
                         .set_type("JWS")
                         .set_payload_claim("user_id", jwt::claim(player.get_id_str()))
                         .set_payload_claim("email", jwt::claim(player.email))
                         .set_issued_at(std::chrono::system_clock::now())
                         .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours{24})
                         .sign(jwt::algorithm::hs256{JWT_SECRET});

        return token;
    }

    inline RegistrationResult RegisterUser(Core::Data::DataService &dataService, const std::string &email,
                                           const std::string &password)
    {
        if (dataService.find_player_by_email(email))
        {
            return {false, "A user with this email already exists."};
        }

        char hashed_password[crypto_pwhash_STRBYTES];

        if (crypto_pwhash_str(hashed_password, password.c_str(), password.length(), crypto_pwhash_OPSLIMIT_INTERACTIVE,
                              crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
        {
            return {false, "Failed to hash password due to a server error."};
        }

        Core::Data::Player new_player;
        new_player.email = email;
        new_player.password_hash = hashed_password;

        dataService.players.insert_one(new_player);
        return {true, "Account created successfully."};
    }

    inline LoginResult LoginUser(Core::Data::DataService &dataService, const std::string &email, const std::string &password)
    {
        auto player_opt = dataService.find_player_by_email(email);
        if (!player_opt)
        {
            return {false, "Invalid email or password."};
        }

        if (crypto_pwhash_str_verify(player_opt->password_hash.c_str(), password.c_str(), password.length()) == 0)
        {
            std::string token = CreateJwtForPlayer(*player_opt);
            return {true, "Login successful.", token};
        }

        return {false, "Invalid email or password."};
    }

} // namespace Core::Tasks