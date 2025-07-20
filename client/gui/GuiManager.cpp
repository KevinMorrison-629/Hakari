#include "client/gui/GuiManager.h"

#include <stdexcept>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include <SDL.h>
#include <SDL_opengl.h>

namespace Client
{
    namespace Gui
    {
        GuiManager::GuiManager() : m_window(nullptr), m_glContext(nullptr), m_isRunning(false) {}

        GuiManager::~GuiManager() { Shutdown(); }

        bool GuiManager::Initialize(const std::string &title, int width, int height)
        {
            if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
            {
                printf("Error: %s\n", SDL_GetError());
                return false;
            }

            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

            auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
            m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);
            if (!m_window)
            {
                return false;
            }

            m_glContext = SDL_GL_CreateContext(m_window);
            SDL_GL_MakeCurrent(m_window, m_glContext);
            SDL_GL_SetSwapInterval(1); // Enable vsync

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();
            (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

            ImGui::StyleColorsDark();

            ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
            ImGui_ImplOpenGL3_Init("#version 130");

            m_isRunning = true;
            return true;
        }

        void GuiManager::Shutdown()
        {
            if (!m_isRunning)
            {
                return;
            }
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext();

            SDL_GL_DeleteContext(m_glContext);
            SDL_DestroyWindow(m_window);
            SDL_Quit();
            m_isRunning = false;
        }

        void GuiManager::ProcessEvents()
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                ImGui_ImplSDL2_ProcessEvent(&event);
                if (event.type == SDL_QUIT)
                {
                    m_isRunning = false;
                }
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(m_window))
                {
                    m_isRunning = false;
                }
            }
        }

        void GuiManager::BeginFrame()
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();
        }

        void GuiManager::EndFrame()
        {
            ImGui::Render();
            SDL_GL_MakeCurrent(m_window, m_glContext);
            glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetRenderData());
            SDL_GL_SwapWindow(m_window);
        }
    } // namespace Gui
} // namespace Client
