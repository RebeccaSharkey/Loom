// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Loom/Core/Application.h"

#include <format>

#include "Loom/Log/Log.h"

namespace Loom
{
    Application::Application() = default;
    Application::~Application() = default;

    void Application::Run()
    {
        LOOM_LOG_NOTICE("Application", "Running Loom Engine...");
        while (true);
    }
}

