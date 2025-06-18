// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Application.h"

#ifdef LOOM_PLATFORM_WINDOWS

#include "Log/Log.h"

#include <iostream>
#include <ostream>
#include <Windows.h>

extern Loom::Application* Loom::CreateApplication();

inline bool bLoggingInitialised = true;

inline void EngineShutdown()
{
    if (bLoggingInitialised)
    {
        LOOM_LOG_NOTICE("Loom", "Shutting down Loom Engine...");
    }

    Loom::Log::Shutdown();
}

inline LONG WINAPI LoomCrashHandler(EXCEPTION_POINTERS* ExceptionInfo) {
    EngineShutdown();
    return EXCEPTION_EXECUTE_HANDLER;
}

inline BOOL WINAPI LoomSignalHandler(DWORD signal) {
    EngineShutdown();
    return TRUE;
}

inline bool EngineStart()
{
    SetConsoleCtrlHandler(LoomSignalHandler, TRUE);
    SetUnhandledExceptionFilter(LoomCrashHandler);

    if (!Loom::Log::Init())
    {
        std::cerr<<"Failed to initialize log system. Aborting."<<std::endl;
        bLoggingInitialised = false;
        return false;
    }

    LOOM_LOG_NOTICE("Loom", "Starting Loom Engine...");

    // Start Engine Code Here.

    return true;
}

int main(int argc, char* argv[])
{
    if (!EngineStart())
    {
        return 0;
    }

    auto app = Loom::CreateApplication();
    app->Run();
    delete app;

    EngineShutdown();
    return 0;
}

#endif