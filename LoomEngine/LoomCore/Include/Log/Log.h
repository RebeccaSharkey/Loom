// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Core.h"
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
        static bool Init();
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
#define LOOM_LOG_DEBUG(tag, ...) \
    LOOM_LOG(Loom::LogLevel::Debug, tag, __VA_ARGS__)
#define LOOM_LOG_INFO(tag, ...) \
    LOOM_LOG(Loom::LogLevel::Info, tag, __VA_ARGS__)
#define LOOM_LOG_NOTICE(tag, ...) \
    LOOM_LOG(Loom::LogLevel::Notice, tag, __VA_ARGS__)
#define LOOM_LOG_WARNING(tag, ...) \
    LOOM_LOG(Loom::LogLevel::Warning, tag, __VA_ARGS__)
#define LOOM_LOG_ERROR(tag, ...) \
    LOOM_LOG(Loom::LogLevel::Error, tag, __VA_ARGS__)
#define LOOM_LOG_CRITICAL(tag, ...) \
    LOOM_LOG(Loom::LogLevel::Critical, tag, __VA_ARGS__)


#define LOOM_LOG_RESET "\033[0m"
#define LOOM_LOG_RED "\033[31m"
#define LOOM_LOG_YELLOW "\033[33m"
#define LOOM_LOG_GREEN "\033[32m"
#define LOOM_LOG_BLUE "\033[34m"
#define LOOM_LOG_MAGENTA "\033[35m"
#define LOOM_LOG_CYAN "\033[36m"
#define LOOM_LOG_WHITE "\033[37m"


