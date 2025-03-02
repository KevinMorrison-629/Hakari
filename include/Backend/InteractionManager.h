#pragma once

#include <dpp/dpp.h>

#include "Persistence/Database.h"

class InteractionManager
{
public:
    InteractionManager();
    ~InteractionManager() = default;

protected:
    static void MakeValidatedUser(const dpp::snowflake &discord_uid);

protected:
    static Database m_Database;
};
