// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "LoomEngine.h"
#include "Window/Window.h"

int main(int argc, char** argv);

namespace Loom
{
    class Window;
    struct PlatformContext;

    struct ApplicationSpecification
    {
        std::string Name = "Loom Application";
        WindowSpecification WindowSpec;

        ApplicationSpecification(const std::string& name = "Loom Application", const WindowSpecification& windowSpec = WindowSpecification())
            : Name(name), WindowSpec(windowSpec)
        {
        }
    };

    class Application
    {
        friend int ::main(int argc, char** argv);
        static Application* Instance;
        OwnerID EventSystemID = GenerateOwnerID();

    public:
        explicit Application(const ApplicationSpecification& spec = ApplicationSpecification());
        virtual ~Application();

        virtual void OnStart() {}
        virtual void OnUpdate(const float DeltaTime) {}
        virtual void OnShutdown() {}

    private:
        bool bIsRunning = true;
        ApplicationSpecification m_Specification;
        std::unique_ptr<PlatformContext> m_PlatformContext;
        std::unique_ptr<Window> m_Window;

        void Run();
        void BindWindowEvents();

        // Window Events
        void OnWindowClosed();
   };

    // To be defined in the client (Editor, Game, App...)
    Application* CreateApplication();
}
