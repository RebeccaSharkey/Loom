// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "Log/LogLevels.h"
#include "Log/LogMessage.h"

namespace Loom
{
    class LOOM_API ILogSink
    {
    public:
        virtual ~ILogSink() = default;

        virtual bool Init (bool bInitEnabled, LogLevel minLogLevel = LogLevel::Quiet);
        virtual void Shutdown ();

        virtual void Log(const LogMessage& message) = 0;
        virtual void Flush() = 0;

        virtual void SetEnabled ( bool bInEnabled ) { bEnabled = bInEnabled; }
        [[nodiscard]] virtual bool IsEnabled() const { return bEnabled; }

    protected:
        bool bEnabled = false;

        LogLevel MinLogLevel{};

    public:
        virtual void SetMinLogLevel(const LogLevel level) { MinLogLevel = level; }
        virtual bool AllowedLogLevel(const LogLevel level) const { return level >= MinLogLevel; }

    protected:
        [[nodiscard]] virtual const char* GetLogLevelString(LogLevel level) const;
        [[nodiscard]] virtual const char* GetLogLevelColour(LogLevel level) const { return ""; };
    };
}

