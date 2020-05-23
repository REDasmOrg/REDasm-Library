#pragma once

#include <rdapi/events.h>
#include <unordered_map>
#include <memory>
#include <mutex>

class EventDispatcher
{
    private:
        using event_lock = std::scoped_lock<std::recursive_mutex>;
        struct EventItem { event_id_t id; void* userdata; RD_EventCallback callback; };
        typedef std::unique_ptr<EventItem> EventItemPtr;
        typedef std::unordered_multimap<event_id_t, EventItemPtr> EventMap;

    public:
        EventDispatcher() = delete;
        template<typename EventArgs, typename ...Args> static void dispatch(event_id_t id, void* sender, Args... args);
        static event_t subscribe(event_id_t id, RD_EventCallback callback, void* userdata);
        static void unsubscribe(event_t e);
        static void unsubscribeAll();

    private:
        static EventMap m_events;
        static std::recursive_mutex m_mutex;
};

template<typename EventArgs, typename ...Args>
void EventDispatcher::dispatch(event_id_t id, void* sender, Args... args)
{
    EventArgs e = { id, sender, args... };
    auto range = m_events.equal_range(id);

    for(auto it = range.first; it != range.second; it++) {
        const auto& event = it->second;
        event->callback(reinterpret_cast<const RDEventArgs*>(&e), event->userdata);
    }
}
