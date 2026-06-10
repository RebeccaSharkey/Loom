// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "EntryPoint.h"
#include "Events/EventDispatcher.h"

class LoomSandbox final : public Loom::Application
{

    Loom::OwnerID EventOwner = Loom::GenerateOwnerID();

public:
    LoomSandbox()
        : Application({
            "Loom Sandbox",
            Loom::WindowSpecification("Loom Sandbox", 1280, 720, false)
        })
    {
    }

    void OnStart() override
    {
        LOOM_LOG_NOTICE("Sandbox", "Sandbox Started.");
    }

    void OnUpdate(const float deltaTime) override
    {

    }

    void OnShutdown() override
    {
        Loom::EventDispatcher::UnsubscribeAllForOwner(EventOwner);
        LOOM_LOG_NOTICE("Sandbox", "Sandbox shutdown.");
    }

};

Loom::Application* Loom::CreateApplication()
{
    return new LoomSandbox();
}
