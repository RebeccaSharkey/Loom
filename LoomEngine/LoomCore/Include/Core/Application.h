// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core.h"
#include "Types.h"
#include "Events/EventMacro.h"
#include "Log/Log.h"

namespace Loom
{
    LOOM_EVENT(ApplicationStartedEvent)

    class LOOM_API Application
    {
        OwnerID EventSystemID = GenerateOwnerID();
        public:
            Application();
            virtual ~Application();

            void OnApplicationStarted(const ApplicationStartedEvent& event)
            {
                LOOM_LOG_NOTICE("Application", "Application started successfully.")
            }

            void Run();
   };

    // To be defined in the client (Editor, Game, App...)
    Application* CreateApplication();
}
