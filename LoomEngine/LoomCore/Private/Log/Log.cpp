// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Log/Log.h"

#include "Log/LogMessage.h"
#include "Log/LogStack.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdarg>
#include <cstring>
#include <functional>
#include <iostream>
#include <thread>
#include <utility>

#include "Log/Sinks/ConsoleSink.h"
#include "Log/Sinks/FileSink.h"

#ifdef LOOM_PLATFORM_WINDOWS
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif

    #include <Windows.h>
#endif

namespace Loom
{
    static std::atomic<bool> bLogInitialized = false;
    std::unique_ptr<ConsoleSink> Log::s_ConsoleSink;
    std::unique_ptr<FileSink>    Log::s_FileSink;

    namespace
    {
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
    }

    bool Log::Init()
    {
        if (bLogInitialized.load())
        {
            return true;
        }

        EnableVirtualTerminalMode();

        auto consoleSink = std::make_unique<ConsoleSink>();
        if (!consoleSink->Init(true))
        {
            std::fprintf(stderr, "Loom: Console Sink Initialisation Failed\n");
            return false;
        }

        auto fileSink = std::make_unique<FileSink>();
        if (!fileSink->Init(true, LogLevel::Debug))
        {
            std::fprintf(stderr, "Loom: File Sink Initialisation Failed\n");
            return false;
        }

        LogStack::AttachSink(consoleSink.get());
        LogStack::AttachSink(fileSink.get());
        s_ConsoleSink = std::move(consoleSink);
        s_FileSink = std::move(fileSink);

        bLogInitialized.store(true);
        return true;
    }

    void Log::Shutdown()
    {
        s_ConsoleSink.reset();
        s_FileSink.reset();
        bLogInitialized.store(false);
    }

    void Log::Write(const LogLevel logLevel, const char *tag, const char *message, ...)
    {
        if (!bLogInitialized.load(std::memory_order_acquire))
        {
            return;
        }

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
        char tagBuffer[TAG_SIZE]{};
        std::snprintf(tagBuffer, TAG_SIZE, "%s", tag);
        tagBuffer[TAG_SIZE-1] = '\0';


        const char* bufferPointer = tempBuffer;
        const char* bufferEnd = tempBuffer + messageLength;

        while (bufferPointer < bufferEnd)
        {
            constexpr size_t MESSAGE_SIZE = 512;

            auto bufferRemaining = static_cast<size_t>(bufferEnd - bufferPointer);
            size_t copyLength = (bufferRemaining < MESSAGE_SIZE - 1) ? bufferRemaining : MESSAGE_SIZE - 1;

            //UTF-8 Safe Truncation
            const char* split = bufferPointer + copyLength;
            while (split > bufferPointer && ((*split & 0xC0) == 0x80))
            {
                split--;
            }

            auto safeLength = static_cast<size_t>(split - bufferPointer);
            if (safeLength == 0)
            {
                safeLength = copyLength;
            }

            LogMessage log{};
            log.Level = logLevel;
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
