// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Core/Application.h"

// Dependencies
#include <chrono>
#include <memory>

// Public Sources
#include "Core/Time.h"
#include "Window/WindowEvents.h"
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

        BindWindowEvents();

        m_PlatformContext = std::make_unique<PlatformContext>();

        m_Window.reset(Window::Create(m_Specification.WindowSpec));
        LOOM_ASSERT(m_Window, "Application::Application - Failed to create window");

        // TODO: Init Renderer
    }

    Application::~Application()
    {
        // TODO: Shutdown Renderer
        m_Window.reset();
        m_PlatformContext.reset();

        Instance = nullptr;

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
            EventBus::Flush();

            OnUpdate(static_cast<float>(deltaTime));
        }

        OnShutdown();
    }

    void Application::BindWindowEvents()
    {
        ApplicationEvents.Subscribe<WindowCloseEvent>([this](const WindowCloseEvent&)
        {
            OnWindowClosed();
        });
    }

    void Application::OnWindowClosed()
    {
        bIsRunning = false;
    }
}

