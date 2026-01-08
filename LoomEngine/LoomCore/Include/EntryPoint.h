// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Application.h"
#include "Log/Log.h"

#ifdef LOOM_PLATFORM_WINDOWS
#include <Windows.h>

extern Loom::Application* Loom::CreateApplication();

int main(int argc, char* argv[])
{
    if (!Loom::Log::Init())
    {
        return 0;
    }

    auto app = Loom::CreateApplication();

    app->Run();

    delete app;

    //return 0;
}

#endif