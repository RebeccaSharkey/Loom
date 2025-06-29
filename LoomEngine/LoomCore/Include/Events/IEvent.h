// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.
#pragma once
#include <string>
#include <vector>

#include "EventType.h"

namespace Loom
{
    class LOOM_API IEvent
    {
    public:
        virtual ~IEvent() = default;

        virtual EventID GetEventTypeID() const = 0;
        virtual const char* GetEventName() const { return "Unnamed Event";};

        // Serialising fields
        virtual std::vector<std::string> GetFieldNames() const { return {}; }
        virtual std::vector<std::string> GetFieldTypes() const { return {}; }
        virtual void* GetFieldPointer(const std::string&) { return nullptr; }
    };
}

