#include "server/Application.h"

#include <iostream>
#include <chrono>
#include <thread>

int main()
{
    Server::Application app = Server::Application();

    app.Initialize(9000);
    app.Run();

    return 0;
}