#include "core/net/ServerManager.h"

#include "core/data/Database.h"
#include "core/data/collection/Card.h"

#include <iostream>
#include <chrono>
#include <thread>

int main()
{
    Core::Net::ServerManager myServer;

    Core::Data::Database::instance().Connect("mongodb://localhost:27017/?maxPoolSize=50", "hakaridb");

    if (!myServer.Start(9000))
    {
        std::cerr << "Failed to start server." << std::endl;
        return 1;
    }

    auto db_client = Core::Data::Database::instance().getClient();

    auto results = db_client.Cards.GetAll({});
    if (!results.empty())
    {
        Core::Data::Collection::Card p = results.front();
        std::cout << p.getName() << std::endl;
    }

    std::cout << "Server started. Press Ctrl+C to exit." << std::endl;

    while (true)
    {
        myServer.Poll();
        myServer.ReceiveMessages();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}