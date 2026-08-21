// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Log/Sinks/FileSink.h"

#include <chrono>
#include <cstring>
#include <ctime>
#include <iostream>
#include <utility>

#ifdef LOOM_PLATFORM_WINDOWS
#include <direct.h>
#else
#include <sys/stat.h>
#endif

namespace Loom
{
    static constexpr uint64_t FLUSH_INTERVAL_MS = 1000;

    FileSink::FileSink(const size_t bufferSize):
        BufferSize(bufferSize < 1 ? 1 : bufferSize),
        FlushThreshold(BufferSize < 2 ? 1 : BufferSize / 2),
        LogBuffer(std::make_unique<LogMessage[]>(BufferSize))
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
        std::tm localTime{};

#ifdef LOOM_PLATFORM_WINDOWS
        if (localtime_s(&localTime, &now) != 0) return false;
#else
        if (!localtime_r(&now, &localTime)) return false;
#endif

        snprintf(LogFileName, sizeof(LogFileName), "Logs/Log--%02d-%02d-%04d--%02d-%02d-%02d.log",
                 localTime.tm_mday, localTime.tm_mon + 1, localTime.tm_year + 1900,
                 localTime.tm_hour, localTime.tm_min, localTime.tm_sec);

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
        {
            std::lock_guard lock(FlushMutex);
            bShutdownRequested = true;
        }

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
        if (!IsEnabled() || !AllowedLogLevel(message.Level))
        {
            return;
        }

        bool bShouldNotify = false;
        {
            std::lock_guard<std::mutex> lock(FlushMutex);
            if (UnflushedCount >= BufferSize)
            {
                ++DroppedCount;
            }
            else
            {
                LogBuffer[CurrentIndex++ % BufferSize] = message;
                ++UnflushedCount;
            }
            bShouldNotify = UnflushedCount >= FlushThreshold;
        }


        if (bShouldNotify)
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
                            return bShutdownRequested || UnflushedCount >= FlushThreshold;
                        });

            if (UnflushedCount > 0)
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

        const size_t start = LastFlushedIndex;
        const size_t end = CurrentIndex;

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

            const char* levelStr = GetLogLevelString(log.Level);

            fprintf(FileHandle, "[%013llu][%-8s][%-12s] %s\n",
                    static_cast<unsigned long long>(log.Timestamp),
                    levelStr,
                    log.Tag,
                    log.Message);
        }

        fflush(FileHandle);
        LastFlushedIndex = end;
        UnflushedCount = 0;

        const size_t dropped = std::exchange(DroppedCount, 0);
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
