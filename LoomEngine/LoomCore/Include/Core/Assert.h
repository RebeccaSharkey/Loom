// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.
#pragma once

#include "Core/Core.h"
#include "Log/Log.h"
#include <csignal>
#include <sstream>

namespace Loom
{
    enum class AssertPolicy
    {
        LogOnly,       // Log and continue
        DebugBreak,    // Log and trigger a debugger break
        Abort,         // Log and abort
        Throw,         // Throw exception (optional use)
        EditorPrompt   // Future: GUI pop-up in LoomEditor
    };

    struct AssertConfig
    {
        static AssertPolicy RuntimePolicy;
    };

    void HandleAssertFailure(const char* expr, const char* file, int line, const char* func, const std::string& message);

    template<typename... Args>
    std::string FormatAssertMessage(Args&&... args)
    {
        std::ostringstream oss;
        ((oss << std::forward<Args>(args)), ...); // Fold expression in C++17+
        return oss.str();
    }
}

// Compiler-safe debug break
#ifdef LOOM_COMPILER_MSVC
#define LOOM_DEBUG_BREAK() __debugbreak()
#elif defined(LOOM_COMPILER_CLANG)
#define LOOM_DEBUG_BREAK() __builtin_debugtrap()
#elif defined(LOOM_COMPILER_GCC)
#define LOOM_DEBUG_BREAK() __builtin_trap()
#else
#define LOOM_DEBUG_BREAK() std::raise(SIGTRAP)
#endif

// TODO: Use HandleAssertFailure function here
// Always-on verification
#define LOOM_VERIFY(expr, ...) \
    do { \
        if (!(expr)) { \
            LOOM_LOG_CRITICAL("Assert", "Assertion Failed: %s\nFile: %s\nLine: %d\nFunction: %s\nMessage: %s", \
                #expr, __FILE__, __LINE__, __func__, __VA_ARGS__); \
            if (Loom::AssertConfig::RuntimePolicy == Loom::AssertPolicy::DebugBreak || \
                Loom::AssertConfig::RuntimePolicy == Loom::AssertPolicy::EditorPrompt) { \
                    LOOM_DEBUG_BREAK(); \
            } \
            else if (Loom::AssertConfig::RuntimePolicy == Loom::AssertPolicy::Abort) { \
                std::abort(); \
            } \
            else if (Loom::AssertConfig::RuntimePolicy == Loom::AssertPolicy::Throw) { \
                throw std::runtime_error("Assert failed: " + Loom::FormatAssertMessage(__VA_ARGS__)); \
            } \
        } \
    } while (0);

// Debug-only assertions
#ifdef LOOM_DEBUG
#define LOOM_ASSERT(expr, ...) LOOM_VERIFY(expr, __VA_ARGS__)
#else
#define LOOM_ASSERT(expr, ...) ((void)(expr))
#endif

