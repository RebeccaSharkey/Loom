// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <string_view>
#include <thread>

#include "LogLevels.h"

enum class LogLevel;

namespace Loom
{
    struct LOOM_API LogMessage
    {
        LogLevel LogLevel;
        char Message[512];
        std::string_view Tag;
        uint64_t Timestamp;
        std::thread::id ThreadID;
    };
}

