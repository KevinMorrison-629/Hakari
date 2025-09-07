#pragma once

#include "server/data/DataService.h"

#include "jwt-cpp/jwt.h"
#include "jwt-cpp/traits/nlohmann-json/traits.h"
#include <memory>
#include <optional>
#include <sodium.h>
#include <string>
#include <vector>

namespace Core::Tasks
{
    // You MUST change this to a long, random, secret string in a config file
    const std::string JWT_SECRET = "a1LS3rL1bN45KWKE7URaSbRZxY8QaAqim2P6wzZ";

    using decoded_token = jwt::decoded_jwt<jwt::traits::nlohmann_json>;

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
        auto token = jwt::create()
                         .set_issuer("HakariBot")
                         .set_type("JWS")
                         .set_payload_claim("user_id", jwt::claim(player.get_id_str()))
                         .set_payload_claim("email", jwt::claim(player.email))
                         .set_issued_at(std::chrono::system_clock::now())
                         .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours{24})
                         .sign(jwt::algorithm::hs256{JWT_SECRET});

        return token;
    }

    inline std::optional<decoded_token> VerifyAndDecodeJwt(const std::string &token)
    {
        try
        {
            // Use nlohmann_json trait for the verifier
            auto verifier = jwt::verify<jwt::traits::nlohmann_json>()
                                .allow_algorithm(jwt::algorithm::hs256{JWT_SECRET})
                                .with_issuer("HakariBot");
            auto decoded = jwt::decode<jwt::traits::nlohmann_json>(token);
            verifier.verify(decoded);
            return decoded;
        }
        catch (const std::exception &e)
        {
            // Token is invalid (expired, wrong signature, etc.)
            return std::nullopt;
        }
    }

    inline RegistrationResult RegisterUser(Core::Data::DataService &dataService, const std::string &email,
                                           const std::string &password, const std::string &displayName)
    {
        if (displayName.length() < 3 || displayName.length() > 16)
        {
            return {false, "Display name must be between 3 and 16 characters."};
        }

        if (dataService.find_player_by_email(email))
        {
            return {false, "A user with this email already exists."};
        }

        if (dataService.find_player_by_display_name(displayName))
        {
            return {false, "A user with this display name already exists."};
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