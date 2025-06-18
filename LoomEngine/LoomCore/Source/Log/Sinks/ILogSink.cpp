// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Log/Sinks/ILogSink.h"

namespace Loom
{
    const char * ILogSink::GetLogLevelString(LogLevel level) const
    {
        switch (level)
        {
            //case LogLevel::Trace:     return LOOM_LOG_WHITE;
            case LogLevel::Debug:       return "DEBUG";
            case LogLevel::Info:        return "INFO";
            case LogLevel::Notice:      return "NOTICE";
            case LogLevel::Warning:     return "WARNING";
            case LogLevel::Error:       return "ERROR";
            case LogLevel::Critical:    return "CRITICAL";
            default:                    return "UNKNOWN";
        }
    }
}
