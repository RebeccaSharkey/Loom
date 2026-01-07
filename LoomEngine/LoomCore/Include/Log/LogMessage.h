// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "LogLevels.h"

namespace Loom
{
    struct alignas(64) LogMessage
    {
        LogLevel LogLevel;
        char Message[512];
        char Tag[32];
        uint64_t Timestamp;
        uint32_t ThreadID;
    };
}

