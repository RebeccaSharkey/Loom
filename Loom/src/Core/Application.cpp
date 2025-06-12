// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Loom/Core/Application.h"
#include <iostream>

namespace Loom
{
    Application::Application() = default;
    Application::~Application() = default;

    void Application::Run()
    {
        std::cout << "Loom Engine Running!" << std::endl;
    }
}

