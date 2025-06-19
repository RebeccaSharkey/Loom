// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Log/Sinks/ILogSink.h"
#include <mutex>

namespace Loom
{
    class LOOM_API ConsoleSink : public ILogSink
    {
        public:
            ~ConsoleSink() override;

            bool Init(bool bInitEnabled, LogLevel minLogLevel = LogLevel::Quiet) override;

            void Log(const LogMessage& message) override;
            void Flush() override;

        protected:
            [[nodiscard]] const char *GetLogLevelColour(LogLevel level) const override;
            std::mutex OutputMutex;
    };
}
