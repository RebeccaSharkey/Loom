// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Log/Sinks/ILogSink.h"

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

namespace Loom
{
    class FileSink : public ILogSink
    {
    public:
        explicit FileSink(const size_t bufferSize = 128);
        ~FileSink() override;

    private:
        std::atomic<bool> bShutdownRequested{false};

        FILE* FileHandle = nullptr;
        char LogFileName[128]{};

        size_t BufferSize;
        size_t FlushThreshold;

        std::unique_ptr<LogMessage[]> LogBuffer;
        size_t CurrentIndex{0};
        size_t LastFlushedIndex{0};
        size_t UnflushedCount{0};
        size_t DroppedCount{0};

        std::thread FlushThread;
        std::mutex FlushMutex;
        std::condition_variable FlushCV;

    public:
        // ILogSink
        bool Init(bool bInitEnabled, LogLevel minLogLevel = LogLevel::Quiet) override;
        void Shutdown() override;

        void Log(const LogMessage& message) override;
        void Flush() override;
        // ~ILogSink

    private:
        void FlushLoop();
        void FlushInternal();
    };
}
