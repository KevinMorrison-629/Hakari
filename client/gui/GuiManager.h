#pragma once

#include <functional>
#include <string>

struct SDL_Window;
typedef void *SDL_GLContext;

namespace Client
{
    namespace Gui
    {
        class GuiManager
        {
        public:
            GuiManager();
            ~GuiManager();

            bool Initialize(const std::string &title, int width, int height);
            void Shutdown();
            void ProcessEvents();
            void BeginFrame();
            void EndFrame();

            bool IsRunning() const { return m_isRunning; }

            std::function<void()> RenderGui;

        private:
            SDL_Window *m_window;
            SDL_GLContext m_glContext;
            bool m_isRunning;
        };
    } // namespace Gui
} // namespace Client
