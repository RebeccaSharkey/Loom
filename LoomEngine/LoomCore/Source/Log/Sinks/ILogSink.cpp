// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Log/Sinks/ILogSink.h"
#include "Log/LogStack.h"

namespace Loom
{
    bool ILogSink::Init(const bool bInitEnabled, const LogLevel minLogLevel)
    {
        bEnabled = bInitEnabled;
        MinLogLevel = minLogLevel;
        return true;
    }

    void ILogSink::Shutdown()
    {
        SetEnabled(false);
        LogStack::DetachSink(this);
    }

    const char * ILogSink::GetLogLevelString(const LogLevel level) const
    {
        switch (level)
        {
            case LogLevel::Quiet:       return "QUIET";
            case LogLevel::Debug:       return "DEBUG";
            case LogLevel::Trace:       return "TRACE";
            case LogLevel::Info:        return "INFO";
            case LogLevel::Notice:      return "NOTICE";
            case LogLevel::Warning:     return "WARNING";
            case LogLevel::Error:       return "ERROR";
            case LogLevel::Critical:    return "CRITICAL";
            default:                    return "UNKNOWN";
        }
    }
}
