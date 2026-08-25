// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.
#pragma once

#include "Core/Types.h"
#include "EventType.h"
#include <utility>

namespace Loom
{
    class EventHandle
    {
    public:
        EventHandle() = default;
        EventHandle(EventID eventID, uint32 index, uint32 generation)
            : m_EventID(eventID), m_Index(index), m_Generation(generation) {};


        bool IsValid() const { return m_Generation != 0; }
        void Invalidate() { m_Generation = 0; }

        EventID GetEventID() const { return m_EventID; }
        uint32  GetIndex() const { return m_Index; }
        uint32  GetGeneration() const { return m_Generation; }

    private:
        EventID m_EventID = 0;
        uint32 m_Index = 0;
        uint32 m_Generation = 0;
    };
}

