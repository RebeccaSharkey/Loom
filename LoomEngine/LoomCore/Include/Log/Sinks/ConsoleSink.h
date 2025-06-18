// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Log/Sinks/ILogSink.h"
#include <mutex>

namespace Loom
{
    class LOOM_API ConsoleSink : public ILogSink
    {
        public:
            void Log(const LogMessage& message) override;
            void Flush() override;

        private:
            const char* GetColourCode(LogLevel level) const;
            std::mutex OutputMutex;
    };
}
