#pragma once

#include <functional>
#include <list>

#define EVENT_CONNECT(sender, event, owner, handler) (sender)->event.pushBack(owner, handler)
#define EVENT_DISCONNECT(sender, event, owner)       (sender)->event.disconnect(owner)

namespace REDasm {

template<typename ...ARGS> struct Event
{
    typedef std::function<void(ARGS...)> HandlerType;
    typedef std::pair<void*, HandlerType> HandlerItem;

    Event() { }
    ~Event() { m_handlers.clear(); }
    Event(const Event& rhs) = delete;
    Event& operator =(const Event& rhs) = delete;
    void operator()(ARGS... args) const { for(const auto& item : m_handlers) item.second(std::forward<ARGS>(args)...); }
    void removeLast() { m_handlers.pop_back(); }
    void disconnect() { m_handlers.clear(); }

    template<typename T> void disconnect(T* owner) {
        auto it = m_handlers.begin();

        while(it != m_handlers.end())
        {
            if(it->first != owner)
            {
                it++;
                continue;
            }

            it = m_handlers.erase(it);
        }
    }

    template<typename T> void pushBack(T* owner, const HandlerType& handler) { m_handlers.emplace_back(owner, handler); }

    private:
        std::list<HandlerItem> m_handlers;
};

typedef Event<> SimpleEvent;

}
