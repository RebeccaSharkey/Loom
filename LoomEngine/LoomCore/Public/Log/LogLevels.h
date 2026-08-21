// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once
#include <cstdint>

namespace Loom
{
    enum class LogLevel : uint8_t
    {
        Quiet,
        Debug,
        Trace,
        Info,
        Notice,
        Warning,
        Error,
        Critical,
        NONE //Don't log anything
    };

// Define what level the engine/project will log to
#ifndef LOOM_COMPILE_LOG_LEVEL
#define LOOM_COMPILE_LOG_LEVEL Loom::LogLevel::Quiet
#endif

    constexpr bool IsLogLevelEnabled(LogLevel level)
    {
        return static_cast<int>(level) >= static_cast<int>(LOOM_COMPILE_LOG_LEVEL);
    }
}

