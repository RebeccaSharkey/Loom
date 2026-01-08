// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "LoomEngine.h"
#include "../../Events/EventMacro.h"

namespace Loom
{
    // Window lifecycle events
    LOOM_EVENT(WindowCloseEvent)
    LOOM_EVENT(WindowFocusEvent)
    LOOM_EVENT(WindowLostFocusEvent)
    LOOM_EVENT(WindowMovedEvent)

    // Window resize event with dimensions
    LOOM_EVENT(WindowResizeEvent,
        (uint32_t, Width),
        (uint32_t, Height))
}
