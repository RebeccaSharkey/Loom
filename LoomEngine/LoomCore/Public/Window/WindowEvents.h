// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Types.h"

namespace Loom
{
    struct WindowCloseEvent {};
    struct WindowGainedFocusEvent {};
    struct WindowLostFocusEvent {};
    struct WindowResizedEvent { uint32 Width, Height; };
}

