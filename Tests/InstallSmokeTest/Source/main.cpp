// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

// Proves an installed Loom is usable from outside the engine repo: public
// headers resolve, both modules link, and the bundled SDL3 is found without
// this project mentioning it.

#include "Log/Log.h"
#include "Render2D.h"

int main()
{
    if (!Loom::Log::Init())
    {
        return 1;
    }

    LOOM_LOG_NOTICE("SmokeTest", "Loom::Core linked and running");

    Loom::Render2D renderer;
    renderer.Test();

    LOOM_LOG_NOTICE("SmokeTest", "Loom::Render2D linked and running");

    Loom::Log::Flush();
    Loom::Log::Shutdown();
    return 0;
}
