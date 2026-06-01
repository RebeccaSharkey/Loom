// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "LoomEngine.h"
#include "Events/EventMacro.h"

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

        virtual void OnStart() {}
        virtual void OnUpdate() {}
        virtual void OnShutdown() {}

        void Close();

    private:
        void Run();
        bool bIsRunning = true;
   };

    // To be defined in the client (Editor, Game, App...)
    Application* CreateApplication();
}
