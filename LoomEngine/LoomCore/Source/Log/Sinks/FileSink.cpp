// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Log/Sinks/FileSink.h"

#include <chrono>
#include <cstring>
#include <ctime>
#include <iostream>

#ifdef LOOM_PLATFORM_WINDOWS
#include <direct.h>
#else
#include <sys/stat.h>
#endif

namespace Loom
{
    static constexpr uint64_t FLUSH_INTERVAL_MS = 1000;

    FileSink::FileSink(const size_t bufferSize):
        BufferSize(bufferSize),
        LogBuffer(std::make_unique<LogMessage[]>(bufferSize))
    {

    }

    FileSink::~FileSink()
    {
        FileSink::Shutdown();
    }

    static void CreateLogDirectory()
    {
#ifdef LOOM_PLATFORM_WINDOWS
        _mkdir("Logs");
#else
        mkdir("Logs", 0775);
#endif
    }

    bool FileSink::Init(const bool bInitEnabled, const LogLevel minLogLevel)
    {
        if (!ILogSink::Init(bInitEnabled, minLogLevel))
        {
            return false;
        };

        //EnableVirtualTerminalMode();
        CreateLogDirectory();

        // Generate Filename
        const time_t now = time(nullptr);
        const tm* timeInfo = localtime(&now);
        snprintf(LogFileName, sizeof(LogFileName), "Logs/Log--%02d-%02d-%04d--%02d-%02d-%02d.log",
                 timeInfo->tm_mday, timeInfo->tm_mon + 1, timeInfo->tm_year + 1900,
                 timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);

        // Try to open/create the file
        FileHandle = fopen(LogFileName, "a");
        if (!FileHandle)
        {
            std::cerr<<"Failed to create Log File: "<< LogFileName << std::endl;
            return false;
        }

        // Start Flush Thread
        bShutdownRequested = false;
        FlushThread = std::thread([this] { FlushLoop();});

        return true;
    }

    void FileSink::Shutdown()
    {
        // Request Shutdown
        bShutdownRequested = true;

        // Notify the flush thread
        FlushCV.notify_one();
        if (FlushThread.joinable())
        {
            // Wait for the background thread to finish
            FlushThread.join();
        }

        // Force a final flush on the current thread (guaranteed safe now!)
        Flush();

        // Close the open file
        if (FileHandle)
        {
            fclose(FileHandle);
            FileHandle = nullptr;
        }

        // Remaining cleanup done for all LogSinks
        ILogSink::Shutdown();
    }

    void FileSink::Log(const LogMessage &message)
    {
        if (!IsEnabled() || !AllowedLogLevel(message.LogLevel))
        {
            return;
        }

        // Drop if the buffer is full (loss mode)
        if (UnflushedCount.load(std::memory_order_acquire) >= BufferSize)
        {
            DroppedCount.fetch_add(1, std::memory_order_relaxed);
            FlushCV.notify_one();
            return;
        }

        const size_t index = CurrentIndex.fetch_add(1, std::memory_order_relaxed) % BufferSize;
        LogBuffer[index] = message;

        if (UnflushedCount.fetch_add(1, std::memory_order_relaxed) >= BufferSize)
        {
            FlushCV.notify_one();
        }
    }

    void FileSink::Flush()
    {
        std::lock_guard lock(FlushMutex);
        FlushInternal();
    }

    void FileSink::FlushLoop()
    {
        while (!bShutdownRequested)
        {
            std::unique_lock lock(FlushMutex);
            FlushCV.wait_for(lock, std::chrono::milliseconds(FLUSH_INTERVAL_MS), [this]
                        {
                            return bShutdownRequested || UnflushedCount.load() >= BufferSize;
                        });

            if (UnflushedCount.load() > 0)
            {
                FlushInternal();
            }
        }
    }

    void FileSink::FlushInternal()
    {
        if (!FileHandle)
        {
            return;
        }

        const size_t start = LastFlushedIndex.load(std::memory_order_acquire);
        const size_t end = CurrentIndex.load(std::memory_order_acquire);

        if (start == end)
        {
            return;
        }

        for (size_t i = start; i < end; ++i)
        {
            const size_t index = i % BufferSize;
            const LogMessage& log = LogBuffer[index];

            if (log.Message[0] == '\0')
            {
                continue;
            }

            const char* levelStr = GetLogLevelString(log.LogLevel);

            fprintf(FileHandle, "[%013llu][%-8s][%-12s] %s\n",
                    static_cast<unsigned long long>(log.Timestamp),
                    levelStr,
                    log.Tag,
                    log.Message);
        }

        fflush(FileHandle);
        LastFlushedIndex.store(end, std::memory_order_release);
        UnflushedCount.store(0, std::memory_order_relaxed);

        const size_t dropped = DroppedCount.exchange(0, std::memory_order_relaxed);
        if (dropped > 0)
        {
            fprintf(FileHandle, "[%013llu][%-8s][%-12s] Dropped %zu log messages due to buffer overflow\n",
                    static_cast<unsigned long long>(std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count()),
                     GetLogLevelString(LogLevel::Warning),
                    "FileSink",
                    dropped);
            fflush(FileHandle);
        }
    }
}
