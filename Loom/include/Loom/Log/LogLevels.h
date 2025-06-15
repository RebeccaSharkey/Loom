// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once
#include <cstdint>

namespace Loom
{
    enum class LogLevel : uint8_t
    {
        DEBUG, // Log only to Console? (Normally only used for testing things...?)
        INFO, // Log to Console and Log File
        NOTICE, // Log to Console and Log File
        WARNING, // Log to Console and Log File
        ERROR, // Log to Console and Log File
        CRITICAL, // CRASH - Log to Console and Log
        NONE //Don't log anything
    };

// Define what level the engine/project will log to
#ifndef LOOM_COMPILE_LOG_LEVEL
#define LOOM_COMPILE_LOG_LEVEL Loom::LogLevel::DEBUG
#endif

    constexpr bool IsLogLevelEnabled(LogLevel level)
    {
        return static_cast<int>(level) >= static_cast<int>(LOOM_COMPILE_LOG_LEVEL);
    }
}

