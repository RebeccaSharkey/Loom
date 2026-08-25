// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Events/EventDispatcher.h"

#include <algorithm>

namespace Loom
{
    void EventDispatcher::Unsubscribe(const EventHandle& handle)
    {
        if (!handle.IsValid()) return;

        std::unique_lock lock(GetListenerMutex());

        auto it = GetChannels().find(handle.GetEventID());
        if (it == GetChannels().end()) return;

        Channel& channel = it->second;
        if (handle.GetIndex() >= channel.Slots.size()) return;

        Slot& slot = channel.Slots[handle.GetIndex()];
        if (slot.Generation != handle.GetGeneration()) return;

        KillSlot(handle.GetEventID(), channel, handle.GetIndex());
    }

    void EventDispatcher::UnsubscribeAll()
    {
        std::unique_lock lock(GetListenerMutex());
        LOOM_ASSERT(GetDispatcherCount() == 0, "UnsubscribeAll() called from inside a listener");

        GetChannels().clear();
        GetPendingFrees().clear();
    }

    void EventDispatcher::UnsubscribeAllForOwner(OwnerID owner)
    {
        if (owner == InvalidOwnerID) return;

        std::lock_guard lock(GetListenerMutex());

        for (auto& [eventID, channel] : GetChannels())
        {
            for (size_t i = 0; i < channel.Slots.size(); ++i)
            {
                if (channel.Slots[i].IsActive() && channel.Slots[i].Owner == owner)
                {
                    KillSlot(eventID, channel, static_cast<uint32>(i));
                }
            }
        }
    }

    void EventDispatcher::Broadcast(const IEvent& event)
    {
        InternalBroadcast(event.GetEventTypeID(), &event);
    }

    void EventDispatcher::Flush()
    {
        std::vector<QueuedEvent> queue;
        {
            std::lock_guard lock(GetQueueMutex());
            std::swap(queue, GetQueue());
        }

        for (const auto& event : queue)
        {
            InternalBroadcast(event.ID, event.Data.get());
        }
    }

    size_t EventDispatcher::GetListenerCount(EventID id)
    {
        std::lock_guard lock(GetListenerMutex());

        auto it = GetChannels().find(id);
        if (it == GetChannels().end()) return 0;

        size_t count = 0;
        for (const Slot& slot : it->second.Slots)
        {
            if (slot.IsActive()) ++count;
        }

        return count;
    }

    size_t EventDispatcher::GetOwnerCount(OwnerID id)
    {
        std::lock_guard lock(GetListenerMutex());

        size_t count = 0;
        for (const auto& [eventID, channel] : GetChannels())
        {
            for (const Slot& slot : channel.Slots)
            {
                if (slot.IsActive() && slot.Owner == id)
                {
                    ++count;
                }
            }
        }

        return count;
    }

    std::unordered_map<EventID, EventDispatcher::Channel>& EventDispatcher::GetChannels()
    {
        static std::unordered_map<EventID, Channel> s_Channels;
        return s_Channels;
    }

    std::recursive_mutex& EventDispatcher::GetListenerMutex()
    {
        static std::recursive_mutex s_ListenerMutex;
        return s_ListenerMutex;
    }

    int& EventDispatcher::GetDispatcherCount()
    {
        static int s_DispatchCount = 0;
        return s_DispatchCount;
    }

    std::vector<std::pair<EventID, uint32>>& EventDispatcher::GetPendingFrees()
    {
        static std::vector<std::pair<EventID, uint32>> s_PendingFrees;
        return s_PendingFrees;
    }

    std::vector<QueuedEvent> & EventDispatcher::GetQueue()
    {
        static std::vector<QueuedEvent> s_Queue;
        return s_Queue;
    }

    std::mutex& EventDispatcher::GetQueueMutex()
    {
        static std::mutex s_QueueMutex;
        return s_QueueMutex;
    }

    EventHandle EventDispatcher::InternalSubscribe(EventID eventID, EventCallbackFn function, OwnerID owner)
    {
        std::lock_guard lock(GetListenerMutex());

        Channel& channel = GetChannels()[eventID];
        uint32 index;
        if (!channel.FreeList.empty())
        {
            index = channel.FreeList.back();
            channel.FreeList.pop_back();
        }
        else
        {
            index = static_cast<uint32>(channel.Slots.size());
            channel.Slots.emplace_back();
        }

        Slot& slot = channel.Slots[index];
        slot.Generation += 1;
        slot.Callback = function;
        slot.Owner = owner;

        channel.Order.push_back(index);

        return EventHandle(eventID, index,  slot.Generation);
    }

    void EventDispatcher::InternalBroadcast(EventID eventID, const void* data)
    {
        std::lock_guard lock(GetListenerMutex());

        auto it = GetChannels().find(eventID);
        if (it == GetChannels().end()) return;

        Channel& channel = it->second;

        const size_t count = channel.Slots.size();

        ++GetDispatcherCount();

        for (size_t i = 0; i < count; ++i)
        {
            Slot& slot = channel.Slots[channel.Order[i]];

            if (!slot.IsActive()) continue;

            slot.Callback(data);
        }

        --GetDispatcherCount();

        if (GetDispatcherCount() == 0)
        {
            auto& pending = GetPendingFrees();
            for (const auto& [pendingID, pendingIndex] : pending)
            {
                auto channelIt = GetChannels().find(pendingID);
                if (channelIt == GetChannels().end())
                {
                    ReclaimSlot(channelIt->second, pendingIndex);
                }
                pending.clear();
            }
        }
    }

    void EventDispatcher::KillSlot(EventID eventID, Channel &channel, uint32 index)
    {
        Slot& slot = channel.Slots[index];

        slot.Generation += 1;
        slot.Callback = nullptr;
        slot.Owner = InvalidOwnerID;

        if (GetDispatcherCount() > 0)
        {
            GetPendingFrees().emplace_back(eventID, index);
        }
        else
        {
            ReclaimSlot(channel, index);
        }
    }

    void EventDispatcher::ReclaimSlot(Channel &channel, uint32 index)
    {
        channel.Order.erase(
            std::remove(channel.Order.begin(), channel.Order.end(), index),
            channel.Order.end());

        channel.FreeList.push_back(index);
    }
}

