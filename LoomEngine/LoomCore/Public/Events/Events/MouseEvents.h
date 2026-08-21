// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Events/EventMacro.h"
#include "Input/MouseCodes.h"

namespace Loom
{
    LOOM_EVENT(MouseMovedEvent, (float32, X), (float32, Y), (float32, DeltaX), (float32, DeltaY))
    LOOM_EVENT(MouseButtonPressedEvent, (MouseButton, Button))
    LOOM_EVENT(MouseButtonReleasedEvent, (MouseButton, Button))
    LOOM_EVENT(MouseScrolledEvent, (float32, XOffset), (float32, YOffset))
}
