// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Core.h"

namespace Loom
{
    class LOOM_API Application
    {
        OwnerID EventSystemID = reinterpret_cast<OwnerID>(this);
        public:
            Application();
            virtual ~Application();

            void Run();
   };

    // To be defined in the client (Editor, Game, App...)
    Application* CreateApplication();
}