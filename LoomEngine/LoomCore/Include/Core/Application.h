// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "LoomEngine.h"
#include "Events/EventHandle.h"
#include "Events/EventMacro.h"
#include "Window/Window.h"

int main(int argc, char** argv);

namespace Loom
{
    class Window;

    struct ApplicationSpecification
    {
        std::string Name = "Loom Application";
        WindowSpecification WindowSpec;

        ApplicationSpecification(const std::string& name = "Loom Application", const WindowSpecification& windowSpec = WindowSpecification())
            : Name(name), WindowSpec(windowSpec)
        {
        }
    };

    class LOOM_API Application
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
        virtual void OnEvent(const IEvent& event);

    private:
        bool bIsRunning = true;
        ApplicationSpecification m_Specification;
        std::unique_ptr<Window> m_Window;

        void Run();
        void BindWindowEvents();
        void OnClose();
   };

    // To be defined in the client (Editor, Game, App...)
    Application* CreateApplication();
}
