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
            LOOM_LOG_QUIET("Log", "Console Log Sink Initiated");
        }
        else
        {
            std::cerr << "Failed to initialize Console Sink." << std::endl;
            return false;
        }

        m_FileSink = new FileSink();
        if (m_FileSink->Init(true, LogLevel::Debug))
        {
            LogStack::AttachSink(m_FileSink);
            LOOM_LOG_QUIET("Log", "File Log Sink Initiated");
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
            m_ConsoleSink->Shutdown();
            delete m_ConsoleSink;
            m_ConsoleSink = nullptr;
        }

        if (m_FileSink)
        {
            m_FileSink->Shutdown();
            delete m_FileSink;
            m_FileSink = nullptr;
        }
    }

    void Log::Write(const LogLevel logLevel, const char *tag, const char *message, ...)
    {
        if (!tag || !message)
        {
            return;
        }

        constexpr size_t TAG_SIZE = 32;
        constexpr size_t TEMP_BUFFER_SIZE = 2048;

        char tempBuffer[TEMP_BUFFER_SIZE];

        va_list args;
        va_start(args, message);
        const int length = vsnprintf(tempBuffer, sizeof(tempBuffer), message, args);
        va_end(args);

        // If there is no message, return
        if (length <= 0)
        {
            return;
        }

        // Cap output at TMP_BUFFER_SIZE to avoid buffer overrun
        const size_t messageLength = static_cast<size_t>(std::min(length, static_cast<int>(TEMP_BUFFER_SIZE - 1)));
        tempBuffer[messageLength] = '\0';

        // Tag handling (ensure null-terminated and no overrun)
        char tagBuffer[TAG_SIZE];
        strncpy(tagBuffer, tag, TAG_SIZE);
        tagBuffer[TAG_SIZE-1] = '\0';


        const char* bufferPointer = tempBuffer;
        const char* bufferEnd = tempBuffer + messageLength;

        while (bufferPointer < bufferEnd)
        {
            constexpr size_t MESSAGE_SIZE = 512;

            size_t bufferRemaining = static_cast<size_t>(bufferEnd - bufferPointer);
            size_t copyLength = (bufferRemaining < MESSAGE_SIZE - 1) ? bufferRemaining : MESSAGE_SIZE - 1;

            //UTF-8 Safe Truncation
            const char* split = bufferPointer + copyLength;
            while (split > bufferPointer && ((*split & 0xC0) == 0x80))
            {
                split--;
            }

            size_t safeLength = static_cast<size_t>(split - bufferPointer);
            if (safeLength == 0)
            {
                safeLength = copyLength;
            }

            LogMessage log{};
            log.LogLevel = logLevel;
            log.Timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
            std::memcpy(log.Tag, tagBuffer, TAG_SIZE);
            std::memcpy(log.Message, bufferPointer, safeLength);
            log.Message[safeLength] = '\0';
            log.ThreadID = static_cast<uint32_t>(std::hash<std::thread::id>()(std::this_thread::get_id()));

            LogStack::Broadcast(log);
            bufferPointer += safeLength;
        }
    }

    void Log::Flush()
    {
        LogStack::ForceFlush();
    }
}
