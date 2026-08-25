// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Events/EventBus.h"

#include <algorithm>
#include <utility>

namespace Loom
{
    void EventBus::Unsubscribe(const EventHandle &handle)
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

    void EventBus::UnsubscribeAll()
    {
        std::unique_lock lock(GetListenerMutex());
        LOOM_ASSERT(GetDispatcherCount() == 0, "UnsubscribeAll() called from inside a listener");

        GetChannels().clear();
        GetPendingFrees().clear();
    }

    void EventBus::Flush()
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

    size_t EventBus::GetListenerCount(EventID id)
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

    std::unordered_map<EventID, EventBus::Channel>& EventBus::GetChannels()
    {
        static std::unordered_map<EventID, Channel> s_Channels;
        return s_Channels;
    }

    std::recursive_mutex& EventBus::GetListenerMutex()
    {
        static std::recursive_mutex s_ListenerMutex;
        return s_ListenerMutex;
    }

    int& EventBus::GetDispatcherCount()
    {
        static int s_DispatchCount = 0;
        return s_DispatchCount;
    }

    std::vector<std::pair<EventID, uint32>>& EventBus::GetPendingFrees()
    {
        static std::vector<std::pair<EventID, uint32>> s_PendingFrees;
        return s_PendingFrees;
    }

    std::vector<QueuedEvent> & EventBus::GetQueue()
    {
        static std::vector<QueuedEvent> s_Queue;
        return s_Queue;
    }

    std::mutex& EventBus::GetQueueMutex()
    {
        static std::mutex s_QueueMutex;
        return s_QueueMutex;
    }

    EventHandle EventBus::InternalSubscribe(EventID eventID, EventCallback function)
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
        slot.Callback = std::move(function);

        channel.Order.push_back(index);

        return EventHandle(eventID, index,  slot.Generation);
    }

    void EventBus::InternalBroadcast(EventID eventID, const void* data)
    {
        std::lock_guard lock(GetListenerMutex());

        auto it = GetChannels().find(eventID);
        if (it == GetChannels().end()) return;

        Channel& channel = it->second;

        const size_t count = channel.Order.size();

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
                if (channelIt != GetChannels().end())
                {
                    ReclaimSlot(channelIt->second, pendingIndex);
                }
            }
            pending.clear();
        }
    }

    void EventBus::KillSlot(EventID eventID, Channel &channel, uint32 index)
    {
        Slot& slot = channel.Slots[index];

        slot.Generation += 1;
        slot.Callback = nullptr;

        if (GetDispatcherCount() > 0)
        {
            GetPendingFrees().emplace_back(eventID, index);
        }
        else
        {
            ReclaimSlot(channel, index);
        }
    }

    void EventBus::ReclaimSlot(Channel &channel, uint32 index)
    {
        channel.Order.erase(
            std::remove(channel.Order.begin(), channel.Order.end(), index),
            channel.Order.end());

        channel.FreeList.push_back(index);
    }
}

