// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Loom/Core/Core.h"
#include "LogLevels.h"

namespace Loom
{
    struct LOOM_API LogMessage
    {
        LogLevel logLevel;
        const char* tag;
        char message[512];
        uint64_t timestamp;
    };

    class LOOM_API Log
    {
    public:
        static void Init();
        static void Shutdown();
        static void Write(LogLevel logLevel, const char* tag, const char* message, ...) __attribute__((format(printf, 3, 4)));
        static void Flush();

    private:
        static void OutputToConsole(LogLevel logLevel, const char* tag, const char* formattedMessage, ...);
    };
}

#define LOOM_LOG(level, tag, ...) \
    do{ if constexpr (Loom::IsLogLevelEnabled(level)) \
    Loom::Log::Write(level, tag, __VA_ARGS__);} \
    while(0);

#define LOOM_DEBUG(tag, ...) \
    LOOM_LOG(Loom::LogLevel::DEBUG, tag, __VA_ARGS__)
#define LOOM_INFO(tag, ...) \
    LOOM_LOG(Loom::LogLevel::INFO, tag, __VA_ARGS__)
#define LOOM_WARNING(tag, ...) \
    LOOM_LOG(Loom::LogLevel::WARNING, tag, __VA_ARGS__)
#define LOOM_ERROR(tag, ...) \
    LOOM_LOG(Loom::LogLevel::ERROR, tag, __VA_ARGS__)
#define LOOM_CRITICAL(tag, ...) \
    LOOM_LOG(Loom::LogLevel::CRITICAL, tag, __VA_ARGS__)
