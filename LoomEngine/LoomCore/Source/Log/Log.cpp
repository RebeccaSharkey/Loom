// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Log/Log.h"

#include "Log/LogMessage.h"
#include "Log/LogStack.h"

#include <cstdarg>
#include <cstring>
#include <iostream>

#include "Log/Sinks/ConsoleSink.h"
#include "Log/Sinks/FileSink.h"

namespace Loom
{
    static std::atomic<bool> bLogInitialized = false;

    static ConsoleSink ConsoleSink;
    static FileSink FileSink;

    bool Log::Init()
    {
        if (bLogInitialized.exchange(true))
        {
            return true;
        }

        if ( ConsoleSink.Init(true) )
        {
            LogStack::AttachSink(&ConsoleSink);
            LOOM_LOG_INFO("Log", "Console Log Sink Initiated");
        }
        else
        {
            std::cerr << "Failed to initialize Console Sink." << std::endl;
            return false;
        }


        if ( FileSink.Init(true) )
        {
            LogStack::AttachSink(&FileSink);
            LOOM_LOG_INFO("Log", "File Log Sink Initiated");
        }
        else
        {
            std::cerr << "Failed to initialize File Sink." << std::endl;
            return false;
        }

        LOOM_LOG_INFO("Log", "Loggers initialized successfully.");
        return true;
    }

    void Log::Shutdown()
    {
        ConsoleSink.Shutdown();
        LogStack::DetachSink(&ConsoleSink);

        FileSink.Shutdown();
        LogStack::DetachSink(&FileSink);
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
            LogMessage logMessage;

            logMessage.LogLevel = logLevel;
            logMessage.Tag = std::string_view(tag);
            logMessage.Timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();

            std::strncpy(logMessage.Message, tempBuffer, sizeof(logMessage.Message));
            logMessage.Message[sizeof(logMessage.Message) - 1] = '\0';

            LogStack::Broadcast(logMessage);
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
        LogStack::ForceFlush();
    }
}
