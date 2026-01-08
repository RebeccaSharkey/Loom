// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Core/Application.h"
#include "Events/EventDispatcher.h"
#include "Window/Window.h"
#include "Window/Events/WindowEvents.h"

namespace Loom
{
    Application* Application::Instance = nullptr;

    Application::Application()
    {
        LOOM_LOG_NOTICE("Loom", "Starting Loom Engine...");

        LOOM_ASSERT(!Instance, "Application already exists");
        Instance = this;

        // TODO: Set Working Directory Here

        WindowSpecification windowSpec;
        windowSpec.Title = "Loom Engine";
        windowSpec.Width = 1280;
        windowSpec.Height = 720;
        windowSpec.VSync = true;

        m_Window.reset(Window::Create(windowSpec));
        LOOM_ASSERT(m_Window, "Failed to create window");

        // Subscribe to window close event
        EventDispatcher::Subscribe<WindowCloseEvent>([this](const WindowCloseEvent&)
        {
            LOOM_LOG_INFO("Application", "Window close requested");
            Close();
        }, EventSystemID);

        // TODO: Initialise Application Events

        // TODO: Initialise Renderer Here
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
        while (bIsRunning && !m_Window->ShouldClose())
        {
            m_Window->ProcessEvents();

            // TODO: Engine Tick

            // TODO: Main Thread

            // TODO: Render
        }
    }

    void Application::Close()
    {
        bIsRunning = false;
    }
}

