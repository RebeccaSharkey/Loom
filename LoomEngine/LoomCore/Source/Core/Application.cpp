// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Core/Application.h"
#include "Events/EventDispatcher.h"

namespace Loom
{
    Application* Application::Instance = nullptr;

    Application::Application()
    {
        LOOM_LOG_NOTICE("Loom", "Starting Loom Engine...");

        LOOM_ASSERT(!Instance, "Application already exists");
        Instance = this;
    }

    Application::~Application()
    {
        EventDispatcher::UnsubscribeAll();

        LOOM_LOG_NOTICE("Loom", "Shutting down Loom Engine...");
        Log::Flush();
        Log::Shutdown();
    }

    void Application::Run()
    {

        OnStart();

        while (bIsRunning)
        {
            OnUpdate();

            EventDispatcher::Flush();

            // TODO: Engine Tick

            // TODO: Main Thread

            // TODO: Render
        }

        OnShutdown();
    }

    void Application::Close()
    {
        bIsRunning = false;
    }
}

