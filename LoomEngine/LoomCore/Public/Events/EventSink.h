// © 2025 Ctrl Alt Delete Games. All rights reserved.
#pragma once

#include "EventBus.h"
#include "EventHandle.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace Loom
{
    class EventSink
    {
    public:
        EventSink() = default;
        ~EventSink() { UnsubscribeAll(); };

        EventSink(const EventSink&) = delete;
        EventSink& operator=(const EventSink&) = delete;

        EventSink(EventSink&& other) noexcept
            : EventHandles(std::move(other.EventHandles))
        {
            other.EventHandles.clear();
        };
        EventSink& operator=(EventSink&& other) noexcept
        {
            if ( this != &other )
            {
                UnsubscribeAll();
                EventHandles = std::move(other.EventHandles);
                other.EventHandles.clear();
            }
            return *this;
        }

        template <typename EventT, typename CallbackT>
        EventHandle Subscribe(CallbackT&& callback, EventPriority priority = EventPriority::Gameplay)
        {
            EventHandle eventHandle = EventBus::Subscribe<EventT>(std::forward<CallbackT>(callback), priority);
            EventHandles.push_back(eventHandle);
            return eventHandle;
        }

        void Unsubscribe(EventHandle eventHandle)
        {
            auto it = std::find_if(EventHandles.begin(), EventHandles.end(), [&](const EventHandle& handle)
            {
                return handle.GetEventID() == eventHandle.GetEventID()
                    && handle.GetIndex() == eventHandle.GetIndex()
                    && handle.GetGeneration() == eventHandle.GetGeneration();
            });

            if (it == EventHandles.end()) return;

            EventBus::Unsubscribe(*it);
            EventHandles.erase(it);
        }

        void UnsubscribeAll()
        {
            for ( const EventHandle& handle : EventHandles )
            {
                EventBus::Unsubscribe(handle);
            }

            EventHandles.clear();
        }

        size_t Count() const { return EventHandles.size(); }

    private:
        std::vector<EventHandle> EventHandles;
    };
}
