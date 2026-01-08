// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Core/Application.h"

#include "Events/EventDispatcher.h"
#include "Events/EventHandle.h"

namespace Loom
{
    Application::Application() = default;
    Application::~Application() = default;

    void Application::Run()
    {
        ScopedEventHandle applicationStartedEvent = EventDispatcher::SubscribeScoped<ApplicationStartedEvent>(
            [this](const ApplicationStartedEvent& event) {
                OnApplicationStarted(event);
            },
            reinterpret_cast<OwnerID>(this));

        EventDispatcher::Broadcast(ApplicationStartedEvent{});

        while (true);
    }
}

