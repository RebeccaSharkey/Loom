// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "LoomEngine.h"

namespace Loom
{
    class Application;

    class LOOM_API Time
    {
        friend class Application;

    public:
        static float GetDeltaTime();
        static double GetTotalTime();
        static uint64 GetFrameCount();

    private:
        static void UpdateTime(double deltaTime);

        static double s_DeltaTime;
        static double s_TotalTime;
        static uint64 s_FrameCount;
    };
}

