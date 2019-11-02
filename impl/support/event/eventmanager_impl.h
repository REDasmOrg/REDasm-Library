#pragma once

#include <redasm/support/event/eventmanager.h>
#include <unordered_map>
#include <mutex>

namespace REDasm {

class EventManagerImpl
{
    public:
        using event_lock = std::lock_guard<std::recursive_mutex>;
        typedef std::unordered_multimap<EventArgs::Id, EventSlot> Subscribers;
        typedef std::pair<EventArgs::Id, EventSlot::Id> Group;
        typedef std::unordered_multimap<EventManager::GroupId, Group> Groups;

    public:
        EventManagerImpl() = delete;
        static void unsubscribe(EventSlot::Id slotid);

    public:
         static std::recursive_mutex m_mutex;
         static Subscribers m_subscribers;
         static Groups m_groups;
};

} // namespace REDasm

