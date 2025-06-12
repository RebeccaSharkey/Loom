// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Loom/Core/Core.h"

namespace Loom
{
    class LOOM_API Application
    {
        public:
            Application();
            ~Application();

            void Run();
   };

    // To be defined in client
    Application* CreateApplication();
}