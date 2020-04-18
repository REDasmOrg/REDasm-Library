#include "eventdispatcher.h"
#include <algorithm>

std::recursive_mutex EventDispatcher::m_mutex;
EventDispatcher::EventMap EventDispatcher::m_events;

event_t EventDispatcher::subscribe(event_id_t id, RD_EventCallback callback, void* userdata)
{
    event_lock lock(m_mutex);

    auto event = std::make_unique<EventItem>();
    event->id = id;
    event->callback = callback;
    event->userdata = userdata;

    auto it = m_events.insert({ id, std::move(event) });
    return reinterpret_cast<event_t>(it->second.get());
}

void EventDispatcher::unsubscribe(event_t e)
{
    event_lock lock(m_mutex);

    EventItem* event = reinterpret_cast<EventItem*>(e);
    auto range = m_events.equal_range(event->id);

    auto it = std::find_if(range.first, range.second, [event](const auto& item) {
        return std::tie(item.second->id, item.second->userdata, item.second->callback) ==
               std::tie(event->id, event->userdata, event->callback);
    });

    if(it != m_events.end()) m_events.erase(it);
}

void EventDispatcher::unsubscribeAll()
{
    event_lock lock(m_mutex);
    m_events.clear();
}
