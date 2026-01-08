// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "LoomEngine.h"
#include "Events/EventMacro.h"

int main(int argc, char** argv);

namespace Loom
{
    LOOM_EVENT(ApplicationStartedEvent)

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

    private:
        bool bIsRunning = true;
   };

    // To be defined in the client (Editor, Game, App...)
    Application* CreateApplication();
}
