#pragma once

#include <dpp/dpp.h>

#include "Persistence/Database.h"

class InteractionManager
{
public:
    InteractionManager();
    ~InteractionManager() = default;

protected:
    // static bool IsValidatedUser(const dpp::snowflake &discord_uid);
    // static std::string GenerateUniqueCode();

protected:
    static Database m_Database;
};