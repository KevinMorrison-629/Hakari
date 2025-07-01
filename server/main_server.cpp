#include "core/ServerManager.h"

#include <iostream>
#include <chrono>
#include <thread>

int main() {
    ServerManager myServer;

    if (!myServer.Start(9000)) {
        std::cerr << "Failed to start server." << std::endl;
        return 1;
    }

    std::cout << "Server started. Press Ctrl+C to exit." << std::endl;

    while (true) {
        myServer.Poll();
        myServer.ReceiveMessages();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}