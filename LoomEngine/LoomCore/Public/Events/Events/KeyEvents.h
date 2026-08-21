// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Events/EventMacro.h"
#include "Input/KeyCodes.h"

namespace Loom
{
    LOOM_EVENT(KeyPressedEvent, (KeyCode, Key), (bool, IsRepeat))
    LOOM_EVENT(KeyReleasedEvent, (KeyCode, Key))
    LOOM_EVENT(TextInputEvent, (uint32, Codepoint))
}
