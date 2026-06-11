// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "Core/Types.h"
#include "Math/Math.h"
#include "Input/KeyCodes.h"
#include "Input/MouseCodes.h"

namespace Loom
{
    class InputAction;

    enum class InputDeviceType : uint8
    {
        Unknown = 0,
        Keyboard,
        Mouse,
        Gamepad
    };

    enum class InputBindingType : uint8
    {
        Key,
        MouseButton,
        MouseX,
        MouseY,
        MouseWheelX,
        MouseWheelY
    };

    enum class InputValueType : uint8
    {
        Bool,
        Axis1D,
        Axis2D
    };

    enum class InputValueAxis : uint8
    {
        X,
        Y
    };

    enum class InputTriggerEvent : uint8
    {
        Started,
        Triggered,
        Completed
    };

    using InputBindingHandle = uint64;
    constexpr InputBindingHandle InvalidInputBindingHandle = 0;

    struct LOOM_API InputBinding
    {
        InputBindingType Type = InputBindingType::Key;
        InputDeviceType Device = InputDeviceType::Keyboard;
        InputValueAxis Axis = InputValueAxis::X;
        KeyCode Key = KeyCode::Unknown;
        MouseButton Button = MouseButton::Unknown;
        float32 Scale = 1.0f;

        static InputBinding FromKey(KeyCode key, float32 scale = 1.0f, InputValueAxis axis = InputValueAxis::X);
        static InputBinding FromMouseButton(MouseButton button, float32 scale = 1.0f);
        static InputBinding FromMouseX(float32 scale = 1.0f);
        static InputBinding FromMouseY(float32 scale = 1.0f);
        static InputBinding FromMouseWheelX(float32 scale = 1.0f);
        static InputBinding FromMouseWheelY(float32 scale = 1.0f);
    };

    struct LOOM_API InputActionValue
    {
        InputValueType Type = InputValueType::Bool;
        float32 X = 0.0f;
        float32 Y = 0.0f;

        bool GetBool() const;
        float32 GetFloat() const;
        Vector2 GetVector2() const;
    };

    struct LOOM_API InputActionEvent
    {
        const InputAction* Action = nullptr;
        InputTriggerEvent Trigger = InputTriggerEvent::Triggered;
        InputActionValue Value;
        InputDeviceType Device = InputDeviceType::Unknown;

        bool GetBool() const;
        float32 GetFloat() const;
        Vector2 GetVector2() const;
    };
}
