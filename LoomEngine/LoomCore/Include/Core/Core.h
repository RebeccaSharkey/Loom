// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#ifdef LOOM_PLATFORM_WINDOWS
    #ifdef LOOM_BUILD_DLL
        #define LOOM_API __declspec(dllexport)
    #else
        #define LOOM_API __declspec(dllimport)
    #endif
#else
    #define LOOM_API
#endif

// Comment to stop logging to Console
#define LOOM_DEBUG

// Comment to stop logging to Console immediately
#define LOOM_FORCE_CONSOLE_FLUSH