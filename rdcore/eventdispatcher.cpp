#include "eventdispatcher.h"

std::atomic_bool EventDispatcher::m_initialized{false};
std::thread EventDispatcher::m_worker;
std::queue<std::unique_ptr<RDEventArgs>> EventDispatcher::m_events;
std::condition_variable EventDispatcher::m_cv;

std::mutex EventDispatcher::m_mutex;
std::unordered_map<void*, EventDispatcher::EventItem> EventDispatcher::m_listeners;

void EventDispatcher::initialize()
{
    m_initialized.store(true);
    m_worker = std::thread(&EventDispatcher::loop);
}

void EventDispatcher::deinitialize()
{
    m_initialized.store(false);
    m_listeners.clear();
    EventDispatcher::unsubscribeAll();
    m_cv.notify_all();

    if(m_worker.joinable())
        m_worker.join();
}

void EventDispatcher::subscribe(void* owner, Callback_Event listener, void* userdata) { m_listeners[owner] = { listener, userdata }; }
void EventDispatcher::unsubscribe(void* owner) { m_listeners.erase(owner); }
void EventDispatcher::unsubscribeAll() { m_listeners.clear(); }

void EventDispatcher::loop()
{
    while(m_initialized.load())
    {
        event_lock lock(m_mutex);

        // Wait until we have events or deinitialized
        m_cv.wait(lock, []() { return !m_events.empty() || !m_initialized.load(); });
        if(m_events.empty() || !m_initialized.load()) continue;

        // After wait, we own the lock
        const auto e = std::move(m_events.front());
        m_events.pop();

        lock.unlock(); // Unlock now that we're done messing with the queue

        for(const auto& [owner, item] : m_listeners)
            item.listener(e.get(), item.userdata);

        lock.lock();
    }
}
