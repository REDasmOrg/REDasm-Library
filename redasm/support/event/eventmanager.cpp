#include "eventmanager.h"
#include <impl/support/event/eventmanager_impl.h>
#include <algorithm>

namespace REDasm {

EventSlot::Id EventManager::subscribe(EventArgs::Id eventid, EventManager::GroupId groupid, EventSlot::Callable&& slot)
{
    EventManagerImpl::event_lock lock(EventManagerImpl::m_mutex);
    auto it = EventManagerImpl::m_subscribers.emplace(eventid, std::move(slot));

    if(groupid) EventManagerImpl::m_groups.insert({groupid, {eventid, it->second.id()}});
    return it->second.id();
}

EventSlot::Id EventManager::subscribe(EventArgs::Id eventid, EventSlot::Callable&& slot) { return EventManager::subscribe(eventid, 0, std::move(slot)); }

void EventManager::trigger(EventArgs::Id eventid, const EventArgs& e)
{
    EventManagerImpl::event_lock lock(EventManagerImpl::m_mutex);
    auto range = EventManagerImpl::m_subscribers.equal_range(eventid);

    std::for_each(range.first, range.second, [&e](const EventManagerImpl::Subscribers::value_type& sub) {
       sub.second(&e);
    });
}

void EventManager::trigger(EventArgs::Id eventid) { EventManager::trigger(eventid, EventArgs()); }
void EventManager::unevent(EventArgs::Id eventid) { EventManagerImpl::event_lock lock(EventManagerImpl::m_mutex); EventManagerImpl::m_subscribers.erase(eventid); }

void EventManager::unslot(EventSlot::Id slotid)
{
    EventManagerImpl::event_lock lock(EventManagerImpl::m_mutex);

    auto git = std::find_if(EventManagerImpl::m_groups.begin(), EventManagerImpl::m_groups.end(), [slotid](const EventManagerImpl::Groups::value_type& item) -> bool {
        return slotid == item.second.second;
    });

    if(git != EventManagerImpl::m_groups.end()) EventManagerImpl::m_groups.erase(git);

    EventManagerImpl::unsubscribe(slotid);
}

void EventManager::ungroup(EventManager::GroupId groupid)
{
    EventManagerImpl::event_lock lock(EventManagerImpl::m_mutex);
    auto range = EventManagerImpl::m_groups.equal_range(groupid);

    for(auto it = range.first; it != range.second; )
    {
        const EventManagerImpl::Group& group = it->second;
        EventManagerImpl::unsubscribe(group.second);
        it = EventManagerImpl::m_groups.erase(it);
    }

    EventManagerImpl::m_groups.erase(groupid);
}

void EventManager::unsubscribe()
{
    EventManagerImpl::event_lock lock(EventManagerImpl::m_mutex);

    EventManagerImpl::m_groups.clear();
    EventManagerImpl::m_subscribers.clear();
}

} // namespace REDasm
