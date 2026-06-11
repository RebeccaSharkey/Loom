// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "EntryPoint.h"
#include "Events/EventDispatcher.h"

class LoomSandbox final : public Loom::Application
{

    Loom::OwnerID EventOwner = Loom::GenerateOwnerID();
    Loom::InputSubsystem Input;

    Loom::InputAction JumpAction{"Jump", Loom::KeyCode::Space};
    Loom::InputAction FireAction{"Fire", Loom::MouseButton::Left};
    Loom::InputAction MoveAction{"Move", Loom::InputValueType::Axis2D};
    Loom::InputAction LookAction{"Look", Loom::InputValueType::Axis2D};

    Loom::InputContext GameplayContext{"Gameplay"};
    Loom::InputContextHandle GameplayContextHandle = Loom::InvalidInputContextHandle;

    Loom::Vector2 MoveInput{};
    Loom::Vector2 LookInput{};

    bool bFireHeld = false;

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
        MoveAction
            .BindKey(Loom::KeyCode::W, 1.0f, Loom::InputValueAxis::Y)
            .BindKey(Loom::KeyCode::S, -1.0f, Loom::InputValueAxis::Y)
            .BindKey(Loom::KeyCode::D, 1.0f, Loom::InputValueAxis::X)
            .BindKey(Loom::KeyCode::A, -1.0f, Loom::InputValueAxis::X);

        LookAction
            .BindMouseX(1.0f)
            .BindMouseY(-1.0f);

        GameplayContext
            .AddAction(JumpAction)
            .AddAction(FireAction)
            .AddAction(MoveAction)
            .AddAction(LookAction);

        GameplayContextHandle = Input.AddContext(GameplayContext);
        Input.BindAction(JumpAction, Loom::InputTriggerEvent::Started, EventOwner, this, &LoomSandbox::OnJumpStarted);
        Input.BindAction(FireAction, Loom::InputTriggerEvent::Started, EventOwner, this, &LoomSandbox::OnFireStarted);
        Input.BindAction(FireAction, Loom::InputTriggerEvent::Completed, EventOwner, this, &LoomSandbox::OnFireCompleted);
        Input.BindAction(MoveAction, Loom::InputTriggerEvent::Triggered, EventOwner, this, &LoomSandbox::OnMove);
        Input.BindAction(MoveAction, Loom::InputTriggerEvent::Completed, EventOwner, this, &LoomSandbox::OnMove);
        Input.BindAction(LookAction, Loom::InputTriggerEvent::Triggered, EventOwner, this, &LoomSandbox::OnLook);

        LOOM_LOG_NOTICE("Sandbox", "Sandbox Started.");
    }

    void OnUpdate(const float deltaTime) override
    {
        (void)deltaTime;
    }

    void OnShutdown() override
    {
        Input.UnbindAllForOwner(EventOwner);
        Input.RemoveContext(GameplayContextHandle);
        Loom::EventDispatcher::UnsubscribeAllForOwner(EventOwner);
        LOOM_LOG_NOTICE("Sandbox", "Sandbox shutdown.");
    }

private:
    void OnJumpStarted(const Loom::InputActionEvent& event)
    {
        LOOM_LOG_INFO("Sandbox", "%s started", event.Action->GetName().c_str());
    }

    void OnFireStarted(const Loom::InputActionEvent&)
    {
        bFireHeld = true;
        LOOM_LOG_INFO("Sandbox", "Fire started");
    }

    void OnFireCompleted(const Loom::InputActionEvent&)
    {
        bFireHeld = false;
        LOOM_LOG_INFO("Sandbox", "Fire completed");
    }

    void OnMove(const Loom::InputActionEvent& event)
    {
        MoveInput = event.GetVector2();
    }

    void OnLook(const Loom::InputActionEvent& event)
    {
        LookInput = event.GetVector2();
    }

};

Loom::Application* Loom::CreateApplication()
{
    return new LoomSandbox();
}
