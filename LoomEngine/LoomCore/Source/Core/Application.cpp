// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Core/Application.h"

#include "Core/Time.h"
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
        auto lastFrameTime = std::chrono::high_resolution_clock::now();

        OnStart();

        while (bIsRunning)
        {
            auto currentTime = std::chrono::high_resolution_clock::now();
            double deltaTime = std::chrono::duration<double>(currentTime - lastFrameTime).count();
            lastFrameTime = currentTime;

            Time::UpdateTime(deltaTime);

            OnUpdate(static_cast<float>(deltaTime));

            EventDispatcher::Flush();
        }

        OnShutdown();
    }

    void Application::Close()
    {
        bIsRunning = false;
    }
}

