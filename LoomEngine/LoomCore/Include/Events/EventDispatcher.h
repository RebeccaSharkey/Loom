// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Types.h"
#include "IEvent.h"
#include "EventHandle.h"
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

namespace Loom
{
    using EventCallbackFn = std::function<void(const void*)>;
    using ListenerID = size_t;

    class LOOM_API EventDispatcher
    {
    public:
        template<typename EventT>
        static EventHandle Subscribe(std::function<void(const EventT&)> callback, OwnerID owner);
        template<typename EventT>
        static ScopedEventHandle SubscribeScoped(std::function<void(const EventT&)> callback, OwnerID owner);

        static void Unsubscribe(const EventHandle& handle);
        static void UnsubscribeAll();
        static void UnsubscribeAllForOwner(OwnerID owner);

        template<typename EventT>
        static void Broadcast(const EventT& event);

        static void Enqueue(EventID eventID, const void* data);
        static void Flush();

        // Debug hooks
        static size_t GetListenerCount(EventID id);
        static size_t GetOwnerCount(OwnerID id);

        struct ListenerEntry
        {
            EventCallbackFn Callback;
            OwnerID Owner = 0;
        };

        struct ListenerGroup
        {
            std::unordered_map<ListenerID, ListenerEntry> Listeners;
            size_t NextID = 1;
        };

    private:
        static std::unordered_map<EventID, ListenerGroup>& GetListenerMap();
        static std::shared_mutex& GetListenerMutex();

        static std::vector<std::pair<EventID, const void*>>& GetQueue();
        static std::mutex& GetQueueMutex();

        static void InternalBroadcast(EventID id, const void* data);
    };

    template<typename EventT>
    EventHandle EventDispatcher::Subscribe(std::function<void(const EventT&)> callback, OwnerID owner)
    {
        static_assert(std::is_base_of_v<IEvent, EventT>, "EventT must derive from IEvent");

        std::unique_lock lock(GetListenerMutex());

        EventID eventId = EventType<EventT>::ID();
        auto& group = GetListenerMap()[eventId];

        ListenerID id = group.NextID++;
        group.Listeners[id] = ListenerEntry {
            [cb = std::move(callback)](const void* raw) {
                cb(*reinterpret_cast<const EventT*>(raw));
            },
            owner
        };

        return EventHandle(eventId, id);
    }

    template<typename EventT>
    ScopedEventHandle EventDispatcher::SubscribeScoped(::std::function<void(const EventT &)> callback, OwnerID owner)
    {
        return ScopedEventHandle(Subscribe<EventT>(std::move(callback), owner));
    }

    template<typename EventT>
    void EventDispatcher::Broadcast(const EventT& event)
    {
        static_assert(std::is_base_of_v<IEvent, EventT>, "EventT must derive from IEvent");
        InternalBroadcast(EventType<EventT>::ID(), &event);
    }

    inline void ScopedEventHandle::Unsubscribe()
    {
        if (Handle.IsValid())
        {
            EventDispatcher::Unsubscribe(Handle);
            Handle.Invalidate();
        }
    }
}
