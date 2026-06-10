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

        BindWindowEvents();
        m_Window->SetEventCallback([this](const IEvent& event)
        {
            OnEvent(event);
        });

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
        EventDispatcher::Subscribe<WindowCreatedEvent>([this] (const WindowCreatedEvent&)
        {
            OnWindowCreated();
        }, EventSystemID);

        EventDispatcher::Subscribe<WindowCloseEvent>([this] (const WindowCloseEvent&)
        {
            OnWindowClosed();
        }, EventSystemID);

        EventDispatcher::Subscribe<WindowFocusEvent>([this] (const WindowFocusEvent&)
        {
            OnWindowFocused();
        }, EventSystemID);

        EventDispatcher::Subscribe<WindowLostFocusEvent>([this] (const WindowLostFocusEvent&)
        {
            OnWindowLostFocus();
        }, EventSystemID);

        EventDispatcher::Subscribe<WindowResizeEvent>([this] (const WindowResizeEvent& event)
        {
            OnWindowResize(event.Width, event.Height);
        }, EventSystemID);

        EventDispatcher::Subscribe<WindowMoveEvent>([this] (const WindowMoveEvent& event)
        {
            OnWindowMoved(event.X, event.Y);
        }, EventSystemID);

        EventDispatcher::Subscribe<WindowMinimizedEvent>([this] (const WindowMinimizedEvent&)
        {
            OnWindowMinimized();
        }, EventSystemID);

        EventDispatcher::Subscribe<WindowMaximizedEvent>([this] (const WindowMaximizedEvent&)
        {
            OnWindowMaximized();
        }, EventSystemID);

        EventDispatcher::Subscribe<WindowRestoredEvent>([this] (const WindowRestoredEvent&)
        {
            OnWindowRestored();
        }, EventSystemID);
    }

    void Application::OnWindowCreated()
    {
    }

    void Application::OnWindowClosed()
    {
        bIsRunning = false;
    }

    void Application::OnWindowFocused()
    {
        LOOM_LOG_INFO("Application", "Window Focused");
    }

    void Application::OnWindowLostFocus()
    {
        LOOM_LOG_INFO("Application", "Window Lost Focus");
    }

    void Application::OnWindowResize(uint32 width, uint32 height)
    {
        LOOM_LOG_INFO("Application", "Window Resized to %dx%d", width, height);
    }

    void Application::OnWindowMoved(int32 x, int32 y)
    {
        LOOM_LOG_INFO("Application", "Window Moved to %d,%d", x, y);
    }

    void Application::OnWindowMinimized()
    {
        LOOM_LOG_INFO("Application", "Window Minimized");
    }

    void Application::OnWindowMaximized()
    {
        LOOM_LOG_INFO("Application", "Window Maximized");
    }

    void Application::OnWindowRestored()
    {
        LOOM_LOG_INFO("Application", "Window Restored");
    }
}

