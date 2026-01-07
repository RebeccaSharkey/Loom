// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <atomic>

namespace Loom
{
    using OwnerID = uint64_t;
    constexpr OwnerID InvalidOwnerID = 0;

    /// Generates a unique owner ID for event system ownership tracking.
    /// Thread-safe. IDs start at 1 (0 is reserved as invalid).
    inline OwnerID GenerateOwnerID()
    {
        static std::atomic<OwnerID> s_Counter{1};
        return s_Counter.fetch_add(1, std::memory_order_relaxed);
    }
}
