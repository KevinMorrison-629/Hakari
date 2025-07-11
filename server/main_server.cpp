#include "server/Application.h"

#include "core/utils/FileReader.h"

#include <chrono>
#include <iostream>
#include <thread>

int main()
{
    Server::Application app = Server::Application();

    std::string botToken = Core::Utils::ReadFile("C:\\Users\\keblm\\Desktop\\Hakari\\bot_token.txt");

    app.Initialize(9000, botToken);
    app.Start();

    return 0;
}