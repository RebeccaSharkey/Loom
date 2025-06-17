// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once
#include <cstdint>

namespace Loom
{
    enum class LogLevel : uint8_t
    {
        Debug, // Log only to Console? (Normally only used for testing things...?)
        Info, // Log to Console and Log File
        Notice, // Log to Console and Log File
        Warning, // Log to Console and Log File
        Error, // Log to Console and Log File
        Critical, // CRASH - Log to Console and Log
        NONE //Don't log anything
    };

// Define what level the engine/project will log to
#ifndef LOOM_COMPILE_LOG_LEVEL
#define LOOM_COMPILE_LOG_LEVEL Loom::LogLevel::Debug
#endif

    constexpr bool IsLogLevelEnabled(LogLevel level)
    {
        return static_cast<int>(level) >= static_cast<int>(LOOM_COMPILE_LOG_LEVEL);
    }
}

