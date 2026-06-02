// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <atomic>

namespace Loom
{
    using int8 = int8_t;
    using int16 = int16_t;
    using int32 = int32_t;
    using int64 = int64_t;

    using uint8 = uint8_t;
    using uint16 = uint16_t;
    using uint32 = uint32_t;
    using uint64 = uint64_t;

    using float32 = float;
    using float64 = double;

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
