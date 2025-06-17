// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Log/Log.h"

#include <atomic>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iostream>

#ifdef LOOM_PLATFORM_WINDOWS
#include <Windows.h>
#include <direct.h>
#else
#include <sys/stat.h>
#endif


namespace Loom
{
    constexpr size_t MAX_LOG_ENTRIES = 1024;
    static LogMessage logMessages_LogBuffer[MAX_LOG_ENTRIES];
    static std::atomic<size_t> logMessages_CurrentIndex = 0;
    static char logMessages_FileName[128] = {};

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
        EnableVirtualTerminalMode();
        CreateLogDirectory();

        const time_t now = time(nullptr);
        const tm* timeInfo = localtime(&now);
        snprintf(logMessages_FileName, sizeof(logMessages_FileName), "Logs/LoomLog_%04d%02d%02d_%02d%02d%02d.log",
                 timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
                 timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);

        FILE* file = fopen(logMessages_FileName, "w");
        if (!file)
        {
            std::cerr<<"Failed to create Log File."<<std::endl;
            return false;
        }

        fclose(file);
        return true;
    }

    void Log::Shutdown()
    {
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
            size_t index = logMessages_CurrentIndex.fetch_add(1, std::memory_order_relaxed) % MAX_LOG_ENTRIES;
            LogMessage& logMessage = logMessages_LogBuffer[index];

            logMessage.logLevel = logLevel;
            logMessage.tag = tag;
            logMessage.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

            snprintf(logMessage.message, sizeof(logMessage.message), "%s", tempBuffer);
            OutputToConsole(logLevel, tag, logMessage.message);

            if ((index + 1) % MAX_LOG_ENTRIES == 0) {
                Flush();
            }
        }
        else
        {
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
        if (logMessages_FileName[0] == '\0')
        {
            return;
        }

        FILE* file = fopen(logMessages_FileName, "w");
        if (!file)
        {
            return;
        }

        for (size_t i = 0; i < MAX_LOG_ENTRIES; ++i)
        {
            const LogMessage& log = logMessages_LogBuffer[i];

            if (log.tag == nullptr || log.message[0] == '\0')
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

            fprintf(file, "[%llu][%s][%s] %s\n",
                    static_cast<unsigned long long>(log.timestamp),
                    levelStr,
                    log.tag,
                    log.message);
        }

        fclose(file);
    }

    void Log::OutputToConsole(LogLevel logLevel, const char *tag, const char *formattedMessage, ...)
    {
        const char* logLevelString = "";
        const char* logColour = "";

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
        fflush(stdout);
    }
}
