// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Core.h"
#include "Sinks/ILogSink.h"

namespace Loom
{
    class LOOM_API LogStack
    {
        public:
            static void AttachSink(ILogSink* sink);

            static void DetachSink(ILogSink* sink);

            static void Broadcast(const LogMessage& message);

            static void ForceFlush();
    };
}
