// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "LoomEngine.h"

namespace Loom
{
    LOOM_EVENT(KeyPressedEvent, (uint16, KeyCode))
    LOOM_EVENT(KeyReleasedEvent, (uint16, KeyCode))
    LOOM_EVENT(TextInputEvent, (uint32, Codepoint))
}
