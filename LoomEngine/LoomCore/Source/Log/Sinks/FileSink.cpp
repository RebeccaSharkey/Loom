// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Log/Sinks/FileSink.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>

#ifdef LOOM_PLATFORM_WINDOWS
#include <direct.h>
#include <Windows.h>
#else
#include <sys/stat.h>
#endif

namespace Loom
{
    static std::atomic<bool> bLogInitialized = false;
    static bool bShutdownRequested = false;

    static constexpr size_t MAX_LOG_ENTRIES = 1024;
    static constexpr size_t FLUSH_THRESHOLD = 32;
    static constexpr uint64_t FLUSH_INTERVAL_MS = 1000;

    static char LogFileName[128] = {};

    static LogMessage LogBuffer[MAX_LOG_ENTRIES];
    static std::atomic<size_t> CurrentIndex = 0;
    static std::atomic<size_t> LastFlushedIndex = 0;

    static std::thread FlushThread;
    static std::atomic<size_t> UnflushedCount = 0;
    static std::mutex FlushMutex;
    static std::condition_variable FlushCV;

    void EnableVirtualTerminalMode()
    {
#ifdef LOOM_PLATFORM_WINDOWS
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;

        if (hOut == INVALID_HANDLE_VALUE || !GetConsoleMode(hOut, &dwMode))
        {
            return;
        }

        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
#endif
    }

    void CreateLogDirectory()
    {
#ifdef LOOM_PLATFORM_WINDOWS
        _mkdir("Logs");
#else
        mkdir("Logs", 0775);
#endif
    }

    bool FileSink::Init(bool bInitEnabled)
    {
        if (!ILogSink::Init(bInitEnabled))
        {
            return false;
        };

        EnableVirtualTerminalMode();
        CreateLogDirectory();

        const time_t now = time(nullptr);
        const tm* timeInfo = localtime(&now);
        snprintf(LogFileName, sizeof(LogFileName), "Logs/Log--%02d-%02d-%04d--%02d-%02d-%02d.log",
                 timeInfo->tm_mday, timeInfo->tm_mon + 1, timeInfo->tm_year + 1900,
                 timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);

        FILE* file = fopen(LogFileName, "w");
        if (!file)
        {
            std::cerr<<"Failed to create Log File."<<std::endl;
            return false;
        }
        fclose(file);

        FlushThread = std::thread([this]
            {
                while (!bShutdownRequested)
                {
                    std::unique_lock lock(FlushMutex);
                    FlushCV.wait_for(lock, std::chrono::milliseconds(FLUSH_INTERVAL_MS), [this]
                        {
                            return bShutdownRequested || UnflushedCount.load() >= FLUSH_THRESHOLD;
                        });

                    if (UnflushedCount.load() > 0)
                    {
                        Flush();
                        UnflushedCount.store(0);
                    }
                }
            });

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
            FlushThread.join();
        }

        Flush();

        ILogSink::Shutdown();
    }

    void FileSink::Log(const LogMessage &message)
    {
        size_t index = CurrentIndex.fetch_add(1, std::memory_order_relaxed) % MAX_LOG_ENTRIES;
        LogMessage& logMessage = LogBuffer[index];
        logMessage.LogLevel = message.LogLevel;
        logMessage.Tag = message.Tag;
        logMessage.Timestamp = message.Timestamp;
        logMessage.ThreadID = std::this_thread::get_id();
        std::strncpy(logMessage.Message, message.Message, sizeof(logMessage.Message));

        if (UnflushedCount.fetch_add(1, std::memory_order_relaxed) >= FLUSH_THRESHOLD)
        {
            FlushCV.notify_one();
        }
    }

    void FileSink::Flush()
    {
        if (LogFileName[0] == '\0')
        {
            return;
        }

        FILE* file = fopen(LogFileName, "a");
        if (!file)
        {
            return;
        }

        const size_t start = LastFlushedIndex.load(std::memory_order_acquire);
        const size_t end = CurrentIndex.load(std::memory_order_acquire);
        for (size_t i = start; i < end; ++i)
        {
            const size_t index = i % MAX_LOG_ENTRIES;
            const LogMessage& log = LogBuffer[index];

            if (log.Message[0] == '\0')
            {
                continue;
            }

            const char* levelStr = GetLogLevelString(log.LogLevel);

            const char* safeTag = log.Tag.data() ? log.Tag.data() : "NULL";

            fprintf(file, "[%013llu][%-8s][%-12s] %s\n",
                    static_cast<unsigned long long>(log.Timestamp),
                    levelStr,
                    safeTag,
                    log.Message);
        }

        LastFlushedIndex.store(end, std::memory_order_release);
        fclose(file);
    }
}