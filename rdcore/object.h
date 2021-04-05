#pragma once

#include <rdapi/events.h>
#include <rdapi/object.h>
#include <unordered_map>
#include <functional>
#include <vector>
#include <string>

#define CPTR(type, ptr) reinterpret_cast<type*>(ptr)
#define CPTR_P(type, ptr) type* p = CPTR(type, ptr)

typedef std::vector<u8> RawData;

class Context;

class Object
{
    protected:
        typedef std::function<void(const RDEventArgs*)> SubscribedListener;
        struct EventItem { SubscribedListener listener; void* userdata; };

    public:
        Object() = default;
        Object(Context* context);
        virtual ~Object() = default;

    public:
        Context* context() const;
        void setContext(Context* ctx);
        void log(const std::string& s) const;
        void status(const std::string& s) const;
        void statusAddress(const std::string& s, rd_address address) const;
        void subscribe(void* owner, const SubscribedListener& listener, void* userdata = nullptr);
        void unsubscribe(void* owner);
        template<typename EventArgs, typename ...Args> void notify(event_id_t id, void* sender, Args... args) const;

    public:
        template<typename Container, typename Function> static void eachMapT(const Container& c, const Function& cb) {
            for(const auto& [key, value] : c) {
                if(!cb(value)) break;
            }
        }

        template<typename Container, typename Function> static void eachT(const Container& c, const Function& cb) {
            for(const auto& item : c) {
                if(!cb(item)) break;
            }
        }

    private:
        Context* m_context{nullptr};
        std::unordered_map<void*, EventItem> m_listeners;
};

template<typename EventArgs, typename ...Args>
void Object::notify(event_id_t id, void* sender, Args... args) const {
    EventArgs e{ id, sender, nullptr, { nullptr }, args... };

    for(const auto& [owner, item] : m_listeners) {
        e.owner = owner;
        e.userdata = item.userdata;
        item.listener(reinterpret_cast<const RDEventArgs*>(&e));
    }
}
