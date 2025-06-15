// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Loom/Log/Log.h"

#include <atomic>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#ifdef _win32
#include <Windows.h>
#endif

namespace Loom
{
    constexpr size_t MAX_LOG_ENTRIES = 1024;
    static LogMessage logMessages_LogBuffer[MAX_LOG_ENTRIES];
    static std::atomic<size_t> logMessages_CurrentIndex = 0;

    void EnableVirtualTerminalMode()
    {
#ifdef _win32
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

    void Log::Init()
    {
        EnableVirtualTerminalMode();
    }

    void Log::Shutdown()
    {
        // TODO: Flush files.
    }

    void Log::Write(LogLevel logLevel, const char *tag, const char *message, ...)
    {
        char tempBuffer[1024];

        va_list args;
        va_start(args, message);
        int formattedMessage = vsnprintf(tempBuffer, sizeof(tempBuffer), message, args);
        va_end(args);

        // Nothing written in the log???? exit.
        if (formattedMessage < 0)
        {
            return;
        }

        if (formattedMessage < 512)
        {
            size_t index = logMessages_CurrentIndex.fetch_add(1, std::memory_order_relaxed) % MAX_LOG_ENTRIES;
            LogMessage& logMessage = logMessages_LogBuffer[index];

            // TODO: Implement these for flushing the logs.
            logMessage.logLevel = logLevel;
            logMessage.tag = tag;
            logMessage.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

            snprintf(logMessage.message, sizeof(logMessage.message), "%s", tempBuffer);
            OutputToConsole(logLevel, tag, logMessage.message);
        }
        else
        {
            // If the message is longer than 512 char, split it into separate logs
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
        // TODO: Write to file.
    }

    void Log::OutputToConsole(LogLevel logLevel, const char *tag, const char *formattedMessage, ...)
    {
        const char* logLevelString = "";
        const char* logColour = "";
        const char* resetColour = "\033[0m";

        switch (logLevel)
        {
            case LogLevel::DEBUG:
                logLevelString = "DEBUG";
                logColour = "\033[37m";
                break;
            case LogLevel::INFO:
                logLevelString = "DEBUG";
                logColour = "\033[37m";
                break;
            case LogLevel::WARNING:
                logLevelString = "WARNING";
                logColour = "\033[33m";
            case LogLevel::ERROR:
                logLevelString = "ERROR";
                logColour = "\033[31m";
                break;
            case LogLevel::CRITICAL:
                logLevelString = "CRITICAL";
                logColour = "\033[1;31m";
                break;
            default:
                logLevelString = "UNKNOWN";
                logColour = resetColour;
        }

        printf("%s[%s][%s] %s%s\n", logColour, logLevelString, tag, formattedMessage, resetColour);
    }
}
