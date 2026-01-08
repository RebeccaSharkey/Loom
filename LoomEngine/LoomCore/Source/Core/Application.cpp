// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Core/Application.h"

#include "Core/Assert.h"
#include "Events/EventDispatcher.h"

namespace Loom
{
    Application* Application::Instance = nullptr;

    Application::Application()
    {
        LOOM_LOG_NOTICE("Loom", "Starting Loom Engine...");

        LOOM_ASSERT(!Instance, "Application already exists");
        Instance = this;

        // TODO: Set Working Directory Here

        // TODO: Create Window

        // TODO: Initialise Application Events

        // TODO: Initialise Renderer Here
    }

    Application::~Application()
    {
        // TODO: Shutdown Renderer

        EventDispatcher::UnsubscribeAll();

        LOOM_LOG_NOTICE("Loom", "Shutting down Loom Engine...");
        Log::Flush();
        Log::Shutdown();
    }

    void Application::Run()
    {
        while (bIsRunning)
        {
            // TODO: Engine Tick

            // TODO: Main Thread

            // TODO: Render

            // TODO: Update Window
        }
    }

    void Application::Close()
    {
        bIsRunning = false;
    }
}

