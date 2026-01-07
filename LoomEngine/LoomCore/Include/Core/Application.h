// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core.h"
#include "Types.h"

namespace Loom
{
    class LOOM_API Application
    {
        OwnerID EventSystemID = GenerateOwnerID();
        public:
            Application();
            virtual ~Application();

            void Run();
   };

    // To be defined in the client (Editor, Game, App...)
    Application* CreateApplication();
}