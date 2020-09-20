#pragma once

// https://embeddedartistry.com/blog/2017/02/08/implementing-an-asynchronous-dispatch-queue

#include <rdapi/events.h>
#include <unordered_map>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>
#include "object.h"

struct RDEventDeleter { void operator()(RDEventArgs* e) const; }; // Cast back for deletion
typedef std::unique_ptr<RDEventArgs, RDEventDeleter> EventPtr;

class EventDispatcher: public Object
{
    private:
        typedef std::unique_lock<std::mutex> EventLock;
        struct EventItem { Callback_Event listener; void* userdata; };

    public:
        EventDispatcher();
        virtual ~EventDispatcher();
        template<typename EventArgs, typename ...Args> void enqueue(event_id_t id, void* sender, Args... args);
        void subscribe(void* owner, Callback_Event listener, void* userdata);
        void unsubscribe(void* owner);
        void unsubscribeAll();

    private:
        void loop();

    private:
        std::atomic_bool m_initialized{false};
        std::thread m_worker;
        std::condition_variable m_cv;
        std::queue<EventPtr> m_events;
        std::unordered_map<void*, EventItem> m_listeners;
        std::mutex m_mutex;
};

template<typename EventArgs, typename ...Args>
void EventDispatcher::enqueue(event_id_t id, void* sender, Args... args)
{
    EventLock lock(m_mutex);

    EventArgs* e = new EventArgs();
    *e = { id, sender, nullptr, { nullptr }, args... };
    m_events.push(EventPtr(reinterpret_cast<RDEventArgs*>(e)));

    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lock.unlock();
    m_cv.notify_all();
}
