// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "Log/LogLevels.h"
#include <memory>

namespace Loom
{
    class ConsoleSink;
    class FileSink;

    class Log
    {
    public:
        static bool Init();
        static void Shutdown();
#if defined(__GNUC__) || defined(__clang__)
        static void Write(LogLevel logLevel, const char* tag, const char* message, ...) __attribute__((format(printf, 3, 4)));
#else
        static void Write(LogLevel logLevel, const char* tag, const char* message, ...);
#endif
        static void Flush();

    private:
        static std::unique_ptr<ConsoleSink> s_ConsoleSink;
        static std::unique_ptr<FileSink>    s_FileSink;
    };
}

#define LOOM_LOG(level, tag, ...) \
    do{ if constexpr (Loom::IsLogLevelEnabled(level)) \
    Loom::Log::Write(level, tag, __VA_ARGS__);} \
    while(0);
#define LOOM_LOG_QUIET(tag, ...) \
    LOOM_LOG(Loom::LogLevel::Quiet, tag, __VA_ARGS__)
#define LOOM_LOG_DEBUG(tag, ...) \
    LOOM_LOG(Loom::LogLevel::Debug, tag, __VA_ARGS__)
#define LOOM_LOG_TRACE(tag, ...) \
    LOOM_LOG(Loom::LogLevel::Trace, tag, __VA_ARGS__)
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





