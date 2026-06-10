// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "LoomEngine.h"

namespace Loom
{

    LOOM_EVENT(WindowCloseEvent)
    LOOM_EVENT(WindowFocusEvent)
    LOOM_EVENT(WindowLostFocusEvent)
    LOOM_EVENT(WindowMoveEvent)

    LOOM_EVENT(WindowResizeEvent, (uint32, Width), (uint32, Height))

}

