#include "client/net/ClientManager.h"

#include <chrono>
#include <iostream>
#include <thread>

int main()
{
    Core::Net::ClientManager myClient;

    myClient.OnMessageReceived = [](const std::string &msg) { std::cout << "Message from server: " << msg << std::endl; };

    if (!myClient.Connect("127.0.0.1:9000"))
    {
        std::cerr << "Failed to connect to server." << std::endl;
        return 1;
    }

    auto lastSendTime = std::chrono::steady_clock::now();

    while (myClient.IsConnected())
    {
        myClient.Poll();
        myClient.ReceiveMessages();

        if (std::chrono::steady_clock::now() - lastSendTime > std::chrono::seconds(3))
        {
            std::cout << "Sending hello message..." << std::endl;
            myClient.SendMessageToServer("Hello from the client!");
            lastSendTime = std::chrono::steady_clock::now();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    std::cout << "Client disconnected." << std::endl;
    return 0;
}