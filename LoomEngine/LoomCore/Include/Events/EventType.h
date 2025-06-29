// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <cstdint>

namespace Loom
{
    using EventID = uint32_t;

    // Template to generate unique EventID per type
    template<typename T>
    struct EventType
    {
        static EventID ID()
        {
            static const EventID ID = Counter++;
            return ID;
        }

    private:
        static inline EventID Counter = 0;
    };
}