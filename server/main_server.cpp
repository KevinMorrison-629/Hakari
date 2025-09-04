#include "server/app/Application.h"

#include "common/core/FileReader.h"

#include <chrono>
#include <iostream>
#include <thread>

int main()
{
    Server::Application app = Server::Application();

    std::string botToken = Core::Utils::ReadFile("C:\\Users\\keblm\\Desktop\\Hakari\\bot_token.txt");

    app.Initialize(9000, 8080, botToken);
    app.Start();

    return 0;
}