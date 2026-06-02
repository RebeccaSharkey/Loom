// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Core/Application.h"

#include "Core/Time.h"
#include "Events/EventDispatcher.h"
#include "Window/Window.h"

namespace Loom
{
    Application* Application::Instance = nullptr;

    Application::Application(const ApplicationSpecification& spec)
        : m_Specification(spec)
    {
        LOOM_LOG_NOTICE("Loom", "Starting Loom Engine...");

        LOOM_ASSERT(!Instance, "Application already exists");
        Instance = this;

        // TODO: Set working directory

        m_Window.reset(Window::Create(spec.WindowSpec));
        LOOM_ASSERT(m_Window, "Application::Application - Failed to create window");

        // TODO: Init Window Events

        // TODO: Init Renderer
    }

    Application::~Application()
    {
        // TODO: Shutdown Renderer

        m_Window.reset();

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
            const double deltaTime = std::chrono::duration<double>(currentTime - lastFrameTime).count();
            lastFrameTime = currentTime;

            Time::UpdateTime(deltaTime);

            OnUpdate(static_cast<float>(deltaTime));

            m_Window->PollEvents();

            if (m_Window->ShouldClose())
            {
                Close();
            }

            EventDispatcher::Flush();
        }

        OnShutdown();
    }

    void Application::Close()
    {
        bIsRunning = false;
    }
}

