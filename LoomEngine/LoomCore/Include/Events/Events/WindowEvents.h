// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "LoomEngine.h"

namespace Loom
{

    LOOM_EVENT(WindowCreatedEvent)
    LOOM_EVENT(WindowCloseEvent)

    LOOM_EVENT(WindowFocusEvent)
    LOOM_EVENT(WindowLostFocusEvent)

    LOOM_EVENT(WindowResizeEvent, (uint32, Width), (uint32, Height))
    LOOM_EVENT(WindowMoveEvent, (uint32, X), (uint32, Y))

    LOOM_EVENT(WindowMinimizedEvent)
    LOOM_EVENT(WindowMaximizedEvent)
    LOOM_EVENT(WindowRestoredEvent)
}

