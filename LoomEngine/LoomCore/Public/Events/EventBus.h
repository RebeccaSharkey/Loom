// © 2025 Ctrl Alt Delete Games. All rights reserved.
#pragma once

#include "LoomEngine.h"
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
    struct QueuedEvent
    {
        EventID ID;
        std::shared_ptr<void> Data;

        template<typename EventT>
        static QueuedEvent Create(const EventT& event)
        {
            return QueuedEvent{
                EventType<EventT>::ID(),
                std::make_shared<EventT>(event)
            };
        }
    };

    class EventBus
    {
        using EventCallback = std::function<void(const void*)>;

        friend class EventSink;

    private:
        template<typename EventT>
        static EventHandle Subscribe(std::function<void(const EventT& event)> callback);

        static void Unsubscribe(const EventHandle& handle);

    public:
        static void UnsubscribeAll();

        template<typename EventT>
        static void Broadcast(const EventT& event);

        template<typename EventT>
        static void Enqueue(const EventT& event);

        static void Flush();

        static size_t GetListenerCount(EventID id);

    private:
        struct Slot
        {
            EventCallback Callback;
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

        static EventHandle InternalSubscribe(EventID eventID, EventCallback callback);
        static void InternalBroadcast(EventID eventID, const void* data);

        static void KillSlot(EventID eventID, Channel& channel, uint32 index);
        static void ReclaimSlot(Channel& channel, uint32 index);

    };

    template<typename EventT>
    EventHandle EventBus::Subscribe(std::function<void(const EventT&)> callback)
    {
        return InternalSubscribe(EventType<EventT>::ID(),
            [cb = std::move(callback)](const void* raw)
            {
                cb(*reinterpret_cast<const EventT*>(raw));
            });
    }

    template<typename EventT>
    void EventBus::Broadcast(const EventT& event)
    {
        InternalBroadcast(EventType<EventT>::ID(), &event);
    }

    template<typename EventT>
    void EventBus::Enqueue(const EventT& event)
    {
        static_assert(std::is_copy_constructible_v<EventT>, "EventT must be copyable to enqueue");

        std::lock_guard lock(GetQueueMutex());
        GetQueue().push_back(QueuedEvent::Create(event));
    }
}
