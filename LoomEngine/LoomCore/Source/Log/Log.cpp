// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Log/Log.h"

#include "Log/LogMessage.h"
#include "Log/LogStack.h"

#include <cstdarg>
#include <cstring>
#include <iostream>

#include "Log/Sinks/ConsoleSink.h"
#include "Log/Sinks/FileSink.h"

#ifdef LOOM_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace Loom
{
    static std::atomic<bool> bLogInitialized = false;

    ConsoleSink* Log::m_ConsoleSink = nullptr;
    FileSink* Log::m_FileSink = nullptr;

    void EnableVirtualTerminalMode()
    {
#ifdef LOOM_PLATFORM_WINDOWS
        HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD doubleWordMode = 0;
        if (outputHandle == INVALID_HANDLE_VALUE || !GetConsoleMode(outputHandle, &doubleWordMode))
        {
            return;
        }
        doubleWordMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(outputHandle, doubleWordMode);
#endif
    }

    bool Log::Init()
    {
        if (bLogInitialized.exchange(true))
        {
            return true;
        }

        m_ConsoleSink = new ConsoleSink();
        if (m_ConsoleSink->Init(true))
        {
            LogStack::AttachSink(m_ConsoleSink);
            LOOM_LOG_INFO("Log", "Console Log Sink Initiated");
        }
        else
        {
            std::cerr << "Failed to initialize Console Sink." << std::endl;
            return false;
        }

        m_FileSink = new FileSink();
        if (m_FileSink->Init(true))
        {
            LogStack::AttachSink(m_FileSink);
            LOOM_LOG_INFO("Log", "File Log Sink Initiated");
        }
        else
        {
            std::cerr << "Failed to initialize File Sink." << std::endl;
            return false;
        }

        EnableVirtualTerminalMode();

        return true;
    }

    void Log::Shutdown()
    {
        if (m_ConsoleSink)
        {
            LogStack::DetachSink(m_ConsoleSink);
            m_ConsoleSink->Shutdown();
            delete m_ConsoleSink;
            m_ConsoleSink = nullptr;
        }

        if (m_FileSink)
        {
            LogStack::DetachSink(m_FileSink);
            m_FileSink->Shutdown();
            delete m_FileSink;
            m_FileSink = nullptr;
        }
    }

    void Log::Write(const LogLevel logLevel, const char *tag, const char *message, ...)
    {
        constexpr size_t TAG_SIZE = 32;

        char tempBuffer[1024];

        va_list args;
        va_start(args, message);
        vsnprintf(tempBuffer, sizeof(tempBuffer), message, args);
        va_end(args);

        // Tag handling (ensure null-terminated and no overrun)
        char tagBuf[TAG_SIZE];
        strncpy(tagBuf, tag, TAG_SIZE);
        tagBuf[TAG_SIZE-1] = '\0';

        // Split and log in MSG_SIZE-1 chunks, UTF-8 safe (basic version)
        const char* bufferPointer = tempBuffer;
        while (*bufferPointer)
        {
            constexpr size_t MSG_SIZE = 512;

            LogMessage log{};
            log.LogLevel = logLevel;
            log.Timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
            strncpy(log.Tag, tag, sizeof(log.Tag));
            log.Tag[TAG_SIZE - 1] = '\0';

            // Copy up to MSG_SIZE - 1
            size_t maxCopy = std::min(strlen(bufferPointer), MSG_SIZE-1);
            size_t actualCopy = maxCopy;
            // TODO: UTF-8 safe truncation
            strncpy(log.Message, bufferPointer, actualCopy);
            log.Message[actualCopy] = '\0';
            log.ThreadID = static_cast<uint32_t>(std::hash<std::thread::id>()(std::this_thread::get_id()));

            LogStack::Broadcast(log);

            bufferPointer += actualCopy;
            if (*bufferPointer == '\0') break;
        }
    }

    void Log::Flush()
    {
        LogStack::ForceFlush();
    }
}
