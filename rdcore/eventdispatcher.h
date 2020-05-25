#pragma once

#include <rdapi/events.h>
#include <unordered_map>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>

class EventDispatcher
{
    private:
        typedef std::unique_lock<std::mutex> event_lock;
        struct EventItem { Callback_Event listener; void* userdata; };

    public:
        EventDispatcher() = delete;
        template<typename EventArgs, typename ...Args> static void enqueue(event_id_t id, void* sender, Args... args);
        static void initialize();
        static void deinitialize();
        static void subscribe(void* owner, Callback_Event listener, void* userdata);
        static void unsubscribe(void* owner);
        static void unsubscribeAll();

    private:
        static void loop();

    private:
        static std::atomic_bool m_initialized;
        static std::thread m_worker;
        static std::condition_variable m_cv;
        static std::queue<std::unique_ptr<RDEventArgs>> m_events;
        static std::unordered_map<void*, EventItem> m_listeners;
        static std::mutex m_mutex;
};

template<typename EventArgs, typename ...Args>
void EventDispatcher::enqueue(event_id_t id, void* sender, Args... args)
{
    event_lock lock(m_mutex);
    EventArgs* e = new EventArgs();
    *e = { id, sender, args... };
    m_events.push(std::unique_ptr<RDEventArgs>(reinterpret_cast<RDEventArgs*>(e)));
    m_cv.notify_all();
}
