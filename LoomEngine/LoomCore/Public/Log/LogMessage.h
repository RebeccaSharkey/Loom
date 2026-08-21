// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "LogLevels.h"
#include "Core/Types.h"

namespace Loom
{
    struct alignas(64) LogMessage
    {
        LogLevel Level;
        char Message[512];
        char Tag[32];
        uint64 Timestamp;
        uint32 ThreadID;
    };
}

