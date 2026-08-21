// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Log/LogStack.h"
#include <vector>
#include <mutex>
#include <algorithm>
#include <iostream>

namespace Loom
{
    namespace
    {
        std::vector<ILogSink*> Sinks;
        std::mutex SinkMutex;
    }

    void LogStack::AttachSink(ILogSink *sink)
    {
        std::lock_guard lock(SinkMutex);
        Sinks.push_back(sink);
    }

    void LogStack::DetachSink(ILogSink *sink)
    {
        std::lock_guard lock(SinkMutex);
        Sinks.erase(std::remove(Sinks.begin(), Sinks.end(), sink), Sinks.end());
    }

    void LogStack::Broadcast(const LogMessage &message)
    {
        std::lock_guard lock(SinkMutex);
        for (auto* sink : Sinks)
        {
            if (sink && sink->IsEnabled())
            {
                sink->Log(message);
            }
        }
    }

    void LogStack::ForceFlush()
    {
        std::lock_guard lock(SinkMutex);
        for (auto* sink : Sinks)
        {
            if (sink && sink->IsEnabled())
            {
                sink->Flush();
            }
        }
    }
}
