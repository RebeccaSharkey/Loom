// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Core/Application.h"

// Dependencies
#include <chrono>
#include <memory>

// Public Sources
#include "Core/Time.h"
#include "Events/IEvent.h"
#include "Events/EventDispatcher.h"
#include "Events/Events/WindowEvents.h"
#include "Window/Window.h"

// Private Sources
#include "../Utilities/PlatformUtils.h"

namespace Loom
{
    Application* Application::Instance = nullptr;

    Application::Application(const ApplicationSpecification &spec)
        :m_Specification(spec)
    {
        // TODO: Set working directory

        LOOM_VERIFY(Log::Init(), "Logging initialization failed");

        LOOM_LOG_NOTICE("Loom", "Starting Loom Engine...");

        LOOM_ASSERT(!Instance, "Application already exists");
        Instance = this;

        m_PlatformContext = std::make_unique<PlatformContext>();

        m_Specification.WindowSpec.EventCallback = [this](const IEvent& event)
        {
            OnEvent(event);
        };
        BindWindowEvents();

        m_Window.reset(Window::Create(m_Specification.WindowSpec));
        LOOM_ASSERT(m_Window, "Application::Application - Failed to create window");

        // TODO: Init Renderer
    }

    Application::~Application()
    {
        Instance = nullptr;

        // TODO: Shutdown Renderer

        m_Window.reset();
        m_PlatformContext.reset();

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
            // Time
            // TODO: Maybe swap for SDL time?
            auto currentTime = std::chrono::high_resolution_clock::now();
            const double deltaTime = std::chrono::duration<double>(currentTime - lastFrameTime).count();
            lastFrameTime = currentTime;
            Time::UpdateTime(deltaTime);

            // Window and Input Events
            m_Window->PollEvents();
            EventDispatcher::Flush();

            OnUpdate(static_cast<float>(deltaTime));
        }

        OnShutdown();
    }

    void Application::OnEvent(const IEvent& event)
    {
        EventDispatcher::Broadcast(event);
    }

    void Application::BindWindowEvents()
    {
        EventDispatcher::Subscribe<WindowCloseEvent>([this] (const WindowCloseEvent&)
        {
            OnWindowClosed();
        }, EventSystemID);
    }

    void Application::OnWindowClosed()
    {
        bIsRunning = false;
    }
}

