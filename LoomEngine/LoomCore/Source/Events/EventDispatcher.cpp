// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Events/EventDispatcher.h"

namespace Loom
{
    static std::unordered_map<EventID, EventDispatcher::ListenerGroup> s_EventListeners;
    static std::shared_mutex s_ListenerMutex;
    static std::vector<std::pair<EventID, const void*>> s_QueuedEvents;
    static std::mutex s_QueueMutex;

    std::unordered_map<EventID, EventDispatcher::ListenerGroup>& EventDispatcher::GetListenerMap()
    {
        return s_EventListeners;
    }

    std::shared_mutex& EventDispatcher::GetListenerMutex()
    {
        return s_ListenerMutex;
    }

    std::vector<std::pair<EventID, const void*>>& EventDispatcher::GetQueue()
    {
        return s_QueuedEvents;
    }

    std::mutex& EventDispatcher::GetQueueMutex()
    {
        return s_QueueMutex;
    }

    void EventDispatcher::Unsubscribe(const EventHandle& handle)
    {
        if (!handle.IsValid()) return;
        std::unique_lock lock(s_ListenerMutex);

        auto it = s_EventListeners.find(handle.GetEventID());
        if (it != s_EventListeners.end())
        {
            it->second.Listeners.erase(handle.GetListenerID());
            if (it->second.Listeners.empty())
                s_EventListeners.erase(it);
        }
    }

    void EventDispatcher::UnsubscribeAll()
    {
        std::unique_lock lock(s_ListenerMutex);
        s_EventListeners.clear();
    }

    void EventDispatcher::UnsubscribeAllForOwner(OwnerID owner)
    {
        std::unique_lock lock(s_ListenerMutex);

        for (auto& [eventID, group] : s_EventListeners)
        {
            for (auto it = group.Listeners.begin(); it != group.Listeners.end(); )
            {
                if (it->second.Owner == owner)
                    it = group.Listeners.erase(it);
                else
                    ++it;
            }
        }
    }

    void EventDispatcher::InternalBroadcast(EventID id, const void* data)
    {
        std::shared_lock lock(s_ListenerMutex);
        auto it = s_EventListeners.find(id);
        if (it != s_EventListeners.end())
        {
            for (const auto& [_, listener] : it->second.Listeners)
                listener.Callback(data);
        }
    }

    void EventDispatcher::Enqueue(EventID eventID, const void* data)
    {
        std::lock_guard lock(s_QueueMutex);
        s_QueuedEvents.emplace_back(eventID, data);
    }

    void EventDispatcher::Flush()
    {
        std::vector<std::pair<EventID, const void*>> queue;
        {
            std::lock_guard lock(s_QueueMutex);
            std::swap(queue, s_QueuedEvents);
        }

        for (const auto& [id, data] : queue)
            InternalBroadcast(id, data);
    }

    size_t EventDispatcher::GetListenerCount(EventID id)
    {
        std::shared_lock lock(s_ListenerMutex);
        auto it = s_EventListeners.find(id);
        return (it != s_EventListeners.end()) ? it->second.Listeners.size() : 0;
    }

    size_t EventDispatcher::GetOwnerCount(OwnerID id)
    {
        std::shared_lock lock(s_ListenerMutex);
        size_t count = 0;
        for (const auto& [_, group] : s_EventListeners)
            for (const auto& [_, entry] : group.Listeners)
                if (entry.Owner == id)
                    ++count;
        return count;
    }
}

