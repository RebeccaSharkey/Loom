// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Core/Application.h"

#include "Core/Time.h"
#include "Events/EventDispatcher.h"
#include "Events/Events/WindowEvents.h"
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
        
        m_Window->SetEventCallback([this](const IEvent& event)
        {
            OnEvent(event);
        });

        BindWindowEvents();

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

            EventDispatcher::Flush();
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
            OnClose();
        }, EventSystemID);
    }

    void Application::OnClose()
    {
        bIsRunning = false;
    }
}

