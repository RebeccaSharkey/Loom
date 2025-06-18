// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Log/Sinks/ILogSink.h"
#include <mutex>

namespace Loom
{
    class LOOM_API FileSink : public ILogSink
    {
    public:
        bool Init(bool bInitEnabled) override;
        void Shutdown() override;

        void Log(const LogMessage& message) override;
        void Flush() override;

    private:
        std::mutex OutputMutex;
    };
}
