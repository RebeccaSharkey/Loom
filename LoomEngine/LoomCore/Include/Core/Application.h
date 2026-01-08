// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "LoomEngine.h"
#include "Events/EventMacro.h"
#include <memory>

int main(int argc, char** argv);

namespace Loom
{
    class Window;

    class LOOM_API Application
    {
        friend int ::main(int argc, char** argv);
        static Application* Instance;
        OwnerID EventSystemID = GenerateOwnerID();

    public:
        Application();
        virtual ~Application();

    private:
        void Run();

    public:
        void Close();
        Window* GetWindow() const { return m_Window.get(); }

    private:
        bool bIsRunning = true;
        std::unique_ptr<Window> m_Window;
   };

    // To be defined in the client (Editor, Game, App...)
    Application* CreateApplication();
}
