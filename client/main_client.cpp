#include "client/gui/GuiManager.h"
#include "client/net/ClientManager.h"
#include "imgui.h"

#include <iostream>
#include <memory>

int main()
{
    auto guiManager = std::make_unique<Client::Gui::GuiManager>();
    if (!guiManager->Initialize("Hakari Client", 1280, 720))
    {
        std::cerr << "Failed to initialize GUI." << std::endl;
        return 1;
    }

    auto clientManager = std::make_unique<Core::Net::ClientManager>();
    clientManager->OnMessageReceived = [](const std::string &msg) { std::cout << "Message from server: " << msg << std::endl; };

    if (!clientManager->Connect("127.0.0.1:9000"))
    {
        std::cerr << "Failed to connect to server." << std::endl;
    }

    guiManager->RenderGui = [&]() {
        ImGui::Begin("Hakari Client");
        if (ImGui::Button("Send 'Hello' to server"))
        {
            if (clientManager->IsConnected())
            {
                clientManager->SendMessageToServer("Hello from the client!");
            }
        }
        ImGui::End();
    };

    while (guiManager->IsRunning())
    {
        clientManager->Poll();
        clientManager->ReceiveMessages();

        guiManager->ProcessEvents();
        guiManager->BeginFrame();
        guiManager->RenderGui();
        guiManager->EndFrame();
    }

    return 0;
}