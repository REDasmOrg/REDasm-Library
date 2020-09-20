#include "eventdispatcher.h"
#include <cassert>

EventDispatcher::EventDispatcher()
{
    m_initialized.store(true);
    m_worker = std::thread(&EventDispatcher::loop, this);
}

EventDispatcher::~EventDispatcher()
{
    EventDispatcher::unsubscribeAll();
    m_initialized.store(false);
    m_cv.notify_all();

    if(m_worker.joinable()) m_worker.join();
}

void EventDispatcher::subscribe(void* owner, Callback_Event listener, void* userdata) { m_listeners[owner] = { listener, userdata }; }
void EventDispatcher::unsubscribe(void* owner) { EventLock lock(m_mutex); m_listeners.erase(owner); }
void EventDispatcher::unsubscribeAll() { EventLock lock(m_mutex); m_listeners.clear(); }

void EventDispatcher::loop()
{
    while(m_initialized.load())
    {
        EventLock lock(m_mutex);

        // Wait until we have events or deinitialized
        m_cv.wait(lock, [&]() { return !m_events.empty() || !m_initialized.load(); });
        if(m_events.empty() || !m_initialized.load()) continue;

        // After wait, we own the lock
        auto e = std::move(m_events.front());
        m_events.pop();

        lock.unlock(); // Unlock now that we're done messing with the queue

        for(const auto& [owner, item] : m_listeners)
        {
            e->owner = owner;
            e->userdata = item.userdata;
            item.listener(e.get());
        }

        lock.lock();
    }
}

void RDEventDeleter::operator()(RDEventArgs* e) const {
    switch(e->eventid) {
        case Event_Error:
            delete reinterpret_cast<RDErrorEventArgs*>(e);
            break;

        case Event_DocumentChanged:
            delete reinterpret_cast<RDDocumentEventArgs*>(e);
            break;

        case Event_CursorPositionChanged:
        case Event_CursorStackChanged:
            delete reinterpret_cast<RDCursorEventArgs*>(e);
            break;

        case Event_BusyChanged:
        case Event_ContextFlagsChanged:
            delete e;
            break;

        default: assert(false);
    }
}
