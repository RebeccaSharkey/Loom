// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <string>

#include "Core/Core.h"

namespace Loom
{
    enum class EventType
    {
        NONE = 0,
        //TODO: (FIRST) Add window open and closing...!
        WINDOW_CLOSE, WINDOW_RESIZE, WINDOW_FOCUS, WINDOW_LOST_FOCUS, WINDOW_MOVED,
        APP_TICK, APP_UPDATE, APP_RENDER, APP_SHUTDOWN,
        KEY_PRESSED, KEY_RELEASED, KEY_TYPED,
        MOUSE_BUTTON_PRESSED, MOUSE_BUTTON_RELEASED, MOUSE_MOVED, MOUSE_SCROLLED,
        //TODO: Add controller input (Future Plan)
    };

    enum EventCategory
    {
        NONE = 0,
        APPLICATION = 1 << 0,
        INPUT = 1 << 1,
        KEYBOARD = 1 << 2,
        MOUSE = 1 << 3,
        MOUSE_BUTTON = 1 << 4,
    };

#define EVENT_CLASS_TYPE(type) \
    static EventType GetStaticType() { return EventType::type; } \
    virtual EventType GetEventType() const override { return GetStaticType();} \
    virtual const char* GetName() const override { #type; }\

#define EVENT_CLASS_CATEGORY(category) \
    virtual int GetCategoryFlags() const override { return category; }

    class LOOM_API Event
    {
        public:
            virtual EventType GetEventType() const = 0;
            virtual const char* GetName() const = 0;
            virtual int GetCategoryFlags() const = 0;
            virtual std::string ToString() const { return GetName();};

            inline bool IsInCategory(EventCategory category)
            {
                return GetCategoryFlags() & category;
            }

        private:
            bool m_Handled = false;

        public:
            [[nodiscard]] bool GetHandled() const;
            void SetHandled(bool handled);
    };
}

