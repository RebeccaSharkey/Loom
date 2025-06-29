// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <cstdint>

//====================
// Platform Detection
//====================
#if defined(_WIN32) || defined(_WIN64)
    #define LOOM_PLATFORM_WINDOWS 1
#elif defined(__APPLE__) || defined(__MACH__)
    #define LOOM_PLATFORM_MACOS 1
#elif defined(__linux__)
    #define LOOM_PLATFORM_LINUX 1
#else
    #error "Unknown platform!"
#endif

//====================
// DLL BUILD TYPE
//====================
#if LOOM_PLATFORM_WINDOWS
    #ifdef LOOM_BUILD_DLL
        #define LOOM_API __declspec(dllexport)
    #else
        #define LOOM_API __declspec(dllimport)
    #endif
#else
    #define LOOM_API
#endif

//====================
// Compiler Detection
//====================
#if defined(_MSC_VER)
    #define LOOM_COMPILER_MSVC 1
#elif defined(__clang__)
    #define LOOM_COMPILER_CLANG 1
#elif defined(__GNUC__)
    #define LOOM_COMPILER_GCC 1
#else
    #error "Unknown compiler!"
#endif

//====================
// Architecture
//====================
#if defined(__x86_64__) || defined(_M_X64)
    #define LOOM_ARCH_X64 1
#elif defined(__i386__) || defined(_M_IX86)
    #define LOOM_ARCH_X86 1
#elif defined(__aarch64__)
    #define LOOM_ARCH_ARM64 1
#elif defined(__arm__) || defined(_M_ARM)
    #define LOOM_ARCH_ARM 1
#else
    #error "Unknown architecture!"
#endif

//====================
// Inlining
//====================
#if LOOM_COMPILER_MSVC
    #define LOOM_FORCE_INLINE __forceinline
#elif LOOM_COMPILER_CLANG || LOOM_COMPILER_GCC
    #define LOOM_FORCE_INLINE inline __attribute__((always_inline))
#else
    #define LOOM_FORCE_INLINE inline
#endif

//====================
// Bit macros
//====================
#define BIT(x) (1u << (x))


namespace Loom
{
    using OwnerID = uintptr_t;
}