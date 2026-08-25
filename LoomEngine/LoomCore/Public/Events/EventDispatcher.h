// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <deque>

#include "LoomEngine.h"
#include "IEvent.h"
#include "EventHandle.h"
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Loom
{
    // Type-erased event storage for queued events
    struct QueuedEvent
    {
        EventID ID;
        std::shared_ptr<void> Data;  // Prevents dangling pointer
        
        template<typename EventT>
        static QueuedEvent Create(const EventT& event)
        {
            return QueuedEvent{
                EventType<EventT>::ID(),
                std::make_shared<EventT>(event)  // Copies the event
            };
        }
    };

    class EventDispatcher
    {
        using EventCallbackFn = std::function<void(const void*)>;

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
        static void Broadcast(const IEvent& event);

        template<typename EventT>
        static void Enqueue(const EventT& event);
        
        static void Flush();

        static size_t GetListenerCount(EventID id);
        static size_t GetOwnerCount(OwnerID id);

    private:

        struct Slot
        {
            EventCallbackFn Callback;
            OwnerID Owner = InvalidOwnerID;
            uint32 Generation = 0;

            bool IsActive() const { return (Generation & 1u) != 0; }
        };

        struct Channel
        {
            std::deque<Slot> Slots;
            std::vector<uint32> Order;
            std::vector<uint32> FreeList;
        };

        static std::unordered_map<EventID, Channel>& GetChannels();
        static std::recursive_mutex& GetListenerMutex();
        static int& GetDispatcherCount();

        static std::vector<std::pair<EventID, uint32>>& GetPendingFrees();

        static std::vector<QueuedEvent>& GetQueue();
        static std::mutex& GetQueueMutex();

        static EventHandle InternalSubscribe(EventID eventID, EventCallbackFn function, OwnerID owner);
        static void InternalBroadcast(EventID eventID, const void* data);

        static void KillSlot(EventID eventID, Channel& channel, uint32 index);
        static void ReclaimSlot(Channel& channel, uint32 index);
    };

    template<typename EventT>
    EventHandle EventDispatcher::Subscribe(std::function<void(const EventT&)> callback, OwnerID owner)
    {
        static_assert(std::is_base_of_v<IEvent, EventT>, "EventT must derive from IEvent");

        std::unique_lock lock(GetListenerMutex());

        return InternalSubscribe(
            EventType<EventT>::ID(),
            [cb = std::move(callback)](const void* raw)
            {
                cb(*reinterpret_cast<const EventT*>(raw));
            },
            owner);
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

    template<typename EventT>
    void EventDispatcher::Enqueue(const EventT& event)
    {
        static_assert(std::is_base_of_v<IEvent, EventT>, "EventT must derive from IEvent");
        static_assert(std::is_copy_constructible_v<EventT>, "EventT must be copyable to enqueue");
        
        std::lock_guard lock(GetQueueMutex());
        GetQueue().push_back(QueuedEvent::Create(event));
    }
    
    inline void ScopedEventHandle::Unsubscribe()
    {
        if (m_Handle.IsValid())
        {
            EventDispatcher::Unsubscribe(m_Handle);
            m_Handle.Invalidate();
        }
    }
}
