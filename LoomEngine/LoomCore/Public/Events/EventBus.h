// © 2025 Ctrl Alt Delete Games. All rights reserved.
#pragma once

#include "LoomEngine.h"
#include "EventHandle.h"

#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Loom
{
    enum class EventPriority : int32
    {
        Engine      = 0,    // Engine
        Debug       = 100,  // Console and Profiler
        UI          = 200,  // Menus and HUDs
        Gameplay    = 300,  // Default
        Late        = 400   // Logging and Analytics
    };

    struct EventContext
    {
        bool Handled = false;

        // TODO: Analytics setup here for "What consumed the event"
    };

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
        using EventCallback = std::function<void(const void*, EventContext&)>;

        friend class EventSink;

    private:
        template<typename EventT, typename CallbackT>
        static EventHandle Subscribe(CallbackT&& callback, EventPriority priority = EventPriority::Gameplay);

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
            EventPriority Priority = EventPriority::Gameplay;
            uint32 Generation = 0;

            bool IsActive() const { return (Generation & 1u) != 0; }
        };

        struct Channel
        {
            std::deque<Slot> Slots;
            std::vector<uint32> Order;
            std::vector<uint32> FreeList;
            bool OrderDirty = false;
        };

        static std::unordered_map<EventID, Channel>& GetChannels();
        static std::recursive_mutex& GetListenerMutex();
        static int& GetDispatcherDepth();

        static std::vector<std::pair<EventID, uint32>>& GetPendingFrees();

        static std::vector<QueuedEvent>& GetQueue();
        static std::mutex& GetQueueMutex();

        static EventHandle InternalSubscribe(EventID eventID, EventCallback callback, EventPriority priority);
        static void InternalBroadcast(EventID eventID, const void* data);

        static void KillSlot(EventID eventID, Channel& channel, uint32 index);
        static void ReclaimSlot(Channel& channel, uint32 index);

    };

    template<typename EventT, typename CallbackT>
    EventHandle EventBus::Subscribe(CallbackT&& callback, EventPriority priority)
    {
        EventCallback tempCallback;

        if constexpr (std::is_invocable_v<CallbackT, const EventT&, EventContext&>)
        {
            tempCallback = [cb = std::forward<CallbackT>(callback)](const void* event, EventContext& context)
            {
                cb(*reinterpret_cast<const EventT*>(event), context);
            };
        }
        else
        {
            static_assert(std::is_invocable_v<CallbackT, const EventT&>,
                "Listener must be callable as (const EventT&) or (const EventT&, EventContext&)");

            tempCallback = [cb = std::forward<CallbackT>(callback)](const void* event, EventContext&)
            {
                cb(*reinterpret_cast<const EventT*>(event));
            };
        }

        return InternalSubscribe(EventType<EventT>::ID(), std::move(tempCallback), priority);
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
