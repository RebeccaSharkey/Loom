// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Core/Time.h"

namespace Loom
{
    double Time::s_DeltaTime = 0.f;
    double Time::s_TotalTime = 0.f;
    uint64 Time::s_FrameCount = 0;

    float Time::GetDeltaTime()
    {
        return static_cast<float>(s_DeltaTime);
    }

    double Time::GetTotalTime()
    {
        return s_TotalTime;
    }

    uint64 Time::GetFrameCount()
    {
        return s_FrameCount;
    }

    void Time::UpdateTime(double deltaTime)
    {
        s_DeltaTime = deltaTime;
        s_TotalTime += deltaTime;
        s_FrameCount++;
    }
}
