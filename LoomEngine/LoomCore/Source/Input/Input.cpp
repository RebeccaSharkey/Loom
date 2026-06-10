// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Input/Input.h"

#include "Events/EventDispatcher.h"
#include "Events/Events/KeyEvents.h"
#include "Events/Events/MouseEvents.h"
#include "Events/Events/WindowEvents.h"

#include <array>
#include <cstddef>

namespace Loom
{
    namespace
    {
        constexpr size_t KeyCount = static_cast<size_t>(KeyCode::Count);
        constexpr size_t MouseButtonCount = static_cast<size_t>(MouseButton::Count);

        struct InputState
        {
            bool Initialized = false;
            OwnerID Owner = InvalidOwnerID;

            std::array<bool, KeyCount> CurrentKeys{};
            std::array<bool, KeyCount> PreviousKeys{};

            std::array<bool, MouseButtonCount> CurrentMouseButtons{};
            std::array<bool, MouseButtonCount> PreviousMouseButtons{};

            float32 MouseX = 0.0f;
            float32 MouseY = 0.0f;
            float32 MouseDeltaX = 0.0f;
            float32 MouseDeltaY = 0.0f;
            float32 MouseScrollX = 0.0f;
            float32 MouseScrollY = 0.0f;
        };

        InputState s_Input;

        size_t ToIndex(KeyCode key)
        {
            return static_cast<size_t>(key);
        }

        size_t ToIndex(MouseButton button)
        {
            return static_cast<size_t>(button);
        }

        bool IsValid(KeyCode key)
        {
            const size_t index = ToIndex(key);
            return key != KeyCode::Unknown && index < KeyCount;
        }

        bool IsValid(MouseButton button)
        {
            const size_t index = ToIndex(button);
            return button != MouseButton::Unknown && index < MouseButtonCount;
        }
    }

    void Input::Initialize()
    {
        if (s_Input.Initialized)
            return;

        s_Input.Owner = GenerateOwnerID();
        s_Input.Initialized = true;

        EventDispatcher::Subscribe<KeyPressedEvent>([](const KeyPressedEvent& event)
        {
            if (IsValid(event.Key))
                s_Input.CurrentKeys[ToIndex(event.Key)] = true;
        }, s_Input.Owner);

        EventDispatcher::Subscribe<KeyReleasedEvent>([](const KeyReleasedEvent& event)
        {
            if (IsValid(event.Key))
                s_Input.CurrentKeys[ToIndex(event.Key)] = false;
        }, s_Input.Owner);

        EventDispatcher::Subscribe<MouseMovedEvent>([](const MouseMovedEvent& event)
        {
            s_Input.MouseX = event.X;
            s_Input.MouseY = event.Y;
            s_Input.MouseDeltaX += event.DeltaX;
            s_Input.MouseDeltaY += event.DeltaY;
        }, s_Input.Owner);

        EventDispatcher::Subscribe<MouseButtonPressedEvent>([](const MouseButtonPressedEvent& event)
        {
            if (IsValid(event.Button))
                s_Input.CurrentMouseButtons[ToIndex(event.Button)] = true;
        }, s_Input.Owner);

        EventDispatcher::Subscribe<MouseButtonReleasedEvent>([](const MouseButtonReleasedEvent& event)
        {
            if (IsValid(event.Button))
                s_Input.CurrentMouseButtons[ToIndex(event.Button)] = false;
        }, s_Input.Owner);

        EventDispatcher::Subscribe<MouseScrolledEvent>([](const MouseScrolledEvent& event)
        {
            s_Input.MouseScrollX += event.XOffset;
            s_Input.MouseScrollY += event.YOffset;
        }, s_Input.Owner);

        EventDispatcher::Subscribe<WindowLostFocusEvent>([](const WindowLostFocusEvent&)
        {
            s_Input.CurrentKeys.fill(false);
            s_Input.CurrentMouseButtons.fill(false);
        }, s_Input.Owner);
    }

    void Input::Shutdown()
    {
        if (!s_Input.Initialized)
            return;

        EventDispatcher::UnsubscribeAllForOwner(s_Input.Owner);
        s_Input = InputState{};
    }

    void Input::BeginFrame()
    {
        s_Input.PreviousKeys = s_Input.CurrentKeys;
        s_Input.PreviousMouseButtons = s_Input.CurrentMouseButtons;
        s_Input.MouseDeltaX = 0.0f;
        s_Input.MouseDeltaY = 0.0f;
        s_Input.MouseScrollX = 0.0f;
        s_Input.MouseScrollY = 0.0f;
    }

    void Input::EndFrame()
    {
    }

    bool Input::IsKeyDown(KeyCode key)
    {
        return IsValid(key) && s_Input.CurrentKeys[ToIndex(key)];
    }

    bool Input::WasKeyPressed(KeyCode key)
    {
        return IsValid(key) && s_Input.CurrentKeys[ToIndex(key)] && !s_Input.PreviousKeys[ToIndex(key)];
    }

    bool Input::WasKeyReleased(KeyCode key)
    {
        return IsValid(key) && !s_Input.CurrentKeys[ToIndex(key)] && s_Input.PreviousKeys[ToIndex(key)];
    }

    bool Input::IsMouseButtonDown(MouseButton button)
    {
        return IsValid(button) && s_Input.CurrentMouseButtons[ToIndex(button)];
    }

    bool Input::WasMouseButtonPressed(MouseButton button)
    {
        return IsValid(button) && s_Input.CurrentMouseButtons[ToIndex(button)] && !s_Input.PreviousMouseButtons[ToIndex(button)];
    }

    bool Input::WasMouseButtonReleased(MouseButton button)
    {
        return IsValid(button) && !s_Input.CurrentMouseButtons[ToIndex(button)] && s_Input.PreviousMouseButtons[ToIndex(button)];
    }

    float32 Input::GetMouseX()
    {
        return s_Input.MouseX;
    }

    float32 Input::GetMouseY()
    {
        return s_Input.MouseY;
    }

    float32 Input::GetMouseDeltaX()
    {
        return s_Input.MouseDeltaX;
    }

    float32 Input::GetMouseDeltaY()
    {
        return s_Input.MouseDeltaY;
    }

    float32 Input::GetMouseScrollX()
    {
        return s_Input.MouseScrollX;
    }

    float32 Input::GetMouseScrollY()
    {
        return s_Input.MouseScrollY;
    }
}
