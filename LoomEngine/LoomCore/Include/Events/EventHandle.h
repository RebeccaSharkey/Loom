// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.
#pragma once

#include "Core/Core.h"
#include "EventType.h"
#include <algorithm>

namespace Loom
{
    class LOOM_API EventHandle
    {
    public:
        EventHandle() = default;
        EventHandle(EventID eventID, size_t listenerID)
            : EventID(eventID), ListenerID(listenerID) {};


        bool IsValid() const { return EventID != 0xFFFFFFFF;};
        void Invalidate() { EventID = InvalidID; };

        EventID GetEventID() const { return EventID; };
        size_t GetListenerID() const { return ListenerID; };


    private:
        static constexpr EventID InvalidID = 0xFFFFFFFF;

        EventID EventID = InvalidID;
        size_t ListenerID = 0;
    };

    class ScopedEventHandle
    {
        public:
        ScopedEventHandle() = default;
        explicit ScopedEventHandle(EventHandle handle)
            : Handle(std::move(handle)) {};

        ScopedEventHandle(ScopedEventHandle&& other) noexcept
                    : Handle(std::move(other.Handle)) {};
        ScopedEventHandle& operator=(ScopedEventHandle&& other) noexcept
        {
            if (this != &other)
            {
                Unsubscribe();
                Handle = std::move(other.Handle);
            }
            return *this;
        }

        ScopedEventHandle(const ScopedEventHandle&) = delete;
        ScopedEventHandle& operator=(const ScopedEventHandle&) = delete;

        ~ScopedEventHandle()
        {
            Unsubscribe();
        }

        void Unsubscribe();

        bool IsValid() const { return Handle.IsValid(); };

    private:
        EventHandle Handle;
    };
}

