// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <cstdint>

namespace Loom
{
    using EventID = uint64_t;

    constexpr EventID ConstHash(const char* string)
    {
        EventID hash = 14695981039346656037ull;
        while (*string)
            hash = (hash ^ static_cast<uint8_t>(*string++)) * 1099511628211ull;
        return hash;
    }

    // Template to generate unique EventID per type
    template<typename T>
    struct EventType
    {
        static constexpr EventID ID()
        {
            return ConstHash(LOOM_TYPE_STRING);
        }

    private:
        static inline EventID Counter = 0;
    };
}