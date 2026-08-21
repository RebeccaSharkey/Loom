// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Log/Sinks/ConsoleSink.h"
#include <cstdio>

namespace Loom
{
    ConsoleSink::~ConsoleSink()
    {
        ILogSink::Shutdown();
    }

    bool ConsoleSink::Init(const bool bInitEnabled, const LogLevel minLogLevel)
    {
        return ILogSink::Init(bInitEnabled, minLogLevel);
    }

    void ConsoleSink::Log(const LogMessage &message)
    {
        if (!IsEnabled() || !AllowedLogLevel(message.LogLevel))
        {
            return;
        }

        std::lock_guard lock(OutputMutex);

        //Format Timestamp
        char timeBuffer[32];
        std::snprintf(timeBuffer, sizeof(timeBuffer), "%llu", static_cast<unsigned long long>(message.Timestamp));
        const char* level = GetLogLevelString(message.LogLevel);

        // Get LogLevel as a string and the output colour attached to that LogLevel
        const char* colour = GetLogLevelColour(message.LogLevel);

        // Format and print full line
        std::printf(
            "%s[%s][%s][%s]: %s\033[0m\n",
            colour,
            timeBuffer,
            level,
            message.Tag,
            message.Message
        );

        Flush();
    }

    void ConsoleSink::Flush()
    {
        std::fflush(stdout);
        std::fflush(stderr);
    }

    const char * ConsoleSink::GetLogLevelColour(const LogLevel level) const
    {
        switch (level)
        {
            case LogLevel::Quiet:       return "\033[37m"; // White
            case LogLevel::Debug:       return "\033[37m"; // White
            case LogLevel::Trace:       return "\033[37m"; // White
            case LogLevel::Info:        return "\033[37m"; // White
            case LogLevel::Notice:      return "\033[36m"; // Cyan
            case LogLevel::Warning:     return "\033[33m"; // Yellow
            case LogLevel::Error:       return "\033[31m"; // Red
            case LogLevel::Critical:    return "\033[31m"; // Red
            default:                    return "\033[0m";  // Reset
        }
    }
}


