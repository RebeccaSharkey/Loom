// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.
#pragma once

#include "Core/Core.h"
#include "EventType.h"
#include <string>
#include <vector>

namespace Loom
{
    class LOOM_API IEvent
    {
    public:
        virtual ~IEvent() = default;

        virtual EventID GetEventTypeID() const = 0;
        virtual const char* GetEventName() const = 0;

        // Serialising fields
        virtual std::vector<std::string> GetFieldNames() const { return {}; }
        virtual std::vector<std::string> GetFieldTypes() const { return {}; }
        virtual void* GetFieldPointer(const std::string&) { return nullptr; }
    };
}

