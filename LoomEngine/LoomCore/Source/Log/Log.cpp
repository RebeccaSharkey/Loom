// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Log/Log.h"

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

#ifdef LOOM_FORCE_CONSOLE_FLUSH
    #define LOOM_FLUSH_STDOUT do { fflush(stdout); } while(0);
#else
    #define LOOM_FLUSH_STDOUT
#endif

namespace Loom
{
    static constexpr size_t MAX_LOG_ENTRIES = 1024;
    static constexpr size_t FLUSH_THRESHOLD = 32;
    static constexpr uint64_t FLUSH_INTERVAL_MS = 1000;

    static std::atomic<bool> bLogInitialized = false;
    static bool bShutdownRequested = false;

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

    bool Log::Init()
    {
        if (bLogInitialized.exchange(true))
        {
            return true;
        }

        EnableVirtualTerminalMode();
        CreateLogDirectory();

        const time_t now = time(nullptr);
        const tm* timeInfo = localtime(&now);
        snprintf(LogFileName, sizeof(LogFileName), "Logs/LoomLog--%02d-%02d-%04d--%02d-%02d-%02d.log",
                 timeInfo->tm_mday, timeInfo->tm_mon + 1, timeInfo->tm_year + 1900,
                 timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);

        FILE* file = fopen(LogFileName, "w");
        if (!file)
        {
            std::cerr<<"Failed to create Log File."<<std::endl;
            return false;
        }
        fclose(file);

        FlushThread = std::thread([]
            {
                while (!bShutdownRequested)
                {
                    std::unique_lock lock(FlushMutex);
                    FlushCV.wait_for(lock, std::chrono::milliseconds(FLUSH_INTERVAL_MS), []
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

        LOOM_LOG_INFO("Log", "Logger initialized successfully.");
        return true;
    }

    void Log::Shutdown()
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
    }

    void Log::Write(LogLevel logLevel, const char *tag, const char *message, ...)
    {
        char tempBuffer[1024];

        va_list args;
        va_start(args, message);
        int formattedMessage = vsnprintf(tempBuffer, sizeof(tempBuffer), message, args);
        va_end(args);

        if (formattedMessage < 0)
        {
            return;
        }

        if (formattedMessage < 512)
        {
            size_t index = CurrentIndex.fetch_add(1, std::memory_order_relaxed) % MAX_LOG_ENTRIES;
            LogMessage& logMessage = LogBuffer[index];

            logMessage.logLevel = logLevel;
            logMessage.tag = std::string_view(tag);
            logMessage.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            std::strncpy(logMessage.message, tempBuffer, sizeof(logMessage.message));
            logMessage.message[sizeof(logMessage.message) - 1] = '\0';


#ifdef LOOM_DEBUG
            OutputToConsole(logLevel, tag, logMessage.message);
#endif

            if (UnflushedCount.fetch_add(1, std::memory_order_relaxed) >= FLUSH_THRESHOLD)
            {
                FlushCV.notify_one();
            }
        }
        else
        {
            // TODO: UTF-8-safe split. Currently assumes ASCII.
            int split = 511;
            while (split > 0 && tempBuffer[split] != ' ')
            {
                split--;
            }

            if (split <= 0)
            {
                split = 511;
            }

            char firstSplit[split];
            memcpy(firstSplit, tempBuffer, split);
            firstSplit[split] = '\0';

            Write(logLevel, tag, firstSplit);
            Write(logLevel, tag, tempBuffer + split + 1);
        }
    }

    void Log::Flush()
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

            if (log.message[0] == '\0')
            {
                continue;
            }

            const char* levelStr = "UNKNOWN";
            switch (log.logLevel)
            {
                case LogLevel::Debug:
                    levelStr = "Debug";
                    break;
                case LogLevel::Info:
                    levelStr = "Info";
                    break;
                case LogLevel::Notice:
                    levelStr = "Notice";
                    break;
                case LogLevel::Warning:
                    levelStr = "Warning";
                    break;
                case LogLevel::Error:
                    levelStr = "Error";
                    break;
                case LogLevel::Critical:
                    levelStr = "Critical";
                    break;
                default:
                    break;
            }

            const char* safeTag = log.tag.data() ? log.tag.data() : "NULL";

            fprintf(file, "[%013llu][%-8s][%-12s] %s\n",
                    static_cast<unsigned long long>(log.timestamp),
                    levelStr,
                    safeTag,
                    log.message);
        }

        LastFlushedIndex.store(end, std::memory_order_release);
        fclose(file);
    }

    void Log::OutputToConsole(const LogLevel logLevel, const char *tag, const char *formattedMessage)
    {
        const char* logLevelString;
        const char* logColour;

        switch (logLevel)
        {
            case LogLevel::Debug:
                logLevelString = "Debug";
                logColour = LOOM_LOG_WHITE;
                break;
            case LogLevel::Info:
                logLevelString = "Info";
                logColour = LOOM_LOG_WHITE;
                break;
            case LogLevel::Notice:
                logLevelString = "Notice";
                logColour = LOOM_LOG_CYAN;
                break;
            case LogLevel::Warning:
                logLevelString = "Warning";
                logColour = LOOM_LOG_YELLOW;
                break;
            case LogLevel::Error:
                logLevelString = "Error";
                logColour = LOOM_LOG_RED;
                break;
            case LogLevel::Critical:
                logLevelString = "Critical";
                logColour = LOOM_LOG_RED;
                break;
            default:
                logLevelString = "UNKNOWN";
                logColour = LOOM_LOG_RESET;
        }

        printf("%s[%s][%s] %s%s\n", logColour, logLevelString, tag, formattedMessage, LOOM_LOG_RESET);
        LOOM_FLUSH_STDOUT;
    }
}
