#include "Backend/InteractionManager.h"

Database InteractionManager::m_Database("hakari");

InteractionManager::InteractionManager()
{
    m_Database.Connect("mongodb://localhost:27017/");
}