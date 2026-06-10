// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "Core/Types.h"
#include "Input/KeyCodes.h"
#include "Input/MouseCodes.h"

namespace Loom
{
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
    };
}
