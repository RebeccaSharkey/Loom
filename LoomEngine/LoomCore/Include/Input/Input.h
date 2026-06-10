// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "Core/Types.h"
#include "Input/KeyCodes.h"
#include "Input/MouseCodes.h"
#include <string>

namespace Loom
{
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

    struct LOOM_API InputBinding
    {
        InputBindingType Type = InputBindingType::Key;
        InputDeviceType Device = InputDeviceType::Keyboard;
        KeyCode Key = KeyCode::Unknown;
        MouseButton Button = MouseButton::Unknown;
        float32 Scale = 1.0f;

        static InputBinding FromKey(KeyCode key, float32 scale = 1.0f);
        static InputBinding FromMouseButton(MouseButton button, float32 scale = 1.0f);
        static InputBinding FromMouseX(float32 scale = 1.0f);
        static InputBinding FromMouseY(float32 scale = 1.0f);
        static InputBinding FromMouseWheelX(float32 scale = 1.0f);
        static InputBinding FromMouseWheelY(float32 scale = 1.0f);
    };

    class LOOM_API Input
    {
    public:
        static void Initialize();
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();

        static bool IsKeyDown(KeyCode key);
        static bool WasKeyPressed(KeyCode key);
        static bool WasKeyReleased(KeyCode key);

        static bool IsMouseButtonDown(MouseButton button);
        static bool WasMouseButtonPressed(MouseButton button);
        static bool WasMouseButtonReleased(MouseButton button);

        static float32 GetMouseX();
        static float32 GetMouseY();
        static float32 GetMouseDeltaX();
        static float32 GetMouseDeltaY();
        static float32 GetMouseScrollX();
        static float32 GetMouseScrollY();
        static InputDeviceType GetLastInputDevice();

        static void CreateContext(const std::string& context);
        static void ClearContext(const std::string& context);
        static void PushContext(const std::string& context);
        static void PopContext();
        static void SetContextEnabled(const std::string& context, bool enabled);
        static bool IsContextEnabled(const std::string& context);

        static void BindAction(const std::string& context, const std::string& action, const InputBinding& binding);
        static void BindAction(const std::string& context, const std::string& action, KeyCode key);
        static void BindAction(const std::string& context, const std::string& action, MouseButton button);
        static void BindAxis(const std::string& context, const std::string& axis, const InputBinding& binding, float32 scale = 1.0f);
        static void BindAxis(const std::string& context, const std::string& axis, KeyCode key, float32 scale);
        static void ClearAction(const std::string& context, const std::string& action);
        static void ClearAxis(const std::string& context, const std::string& axis);

        static bool IsActionDown(const std::string& action);
        static bool WasActionPressed(const std::string& action);
        static bool WasActionReleased(const std::string& action);
        static float32 GetAxis(const std::string& axis);
    };
}
