#pragma once

#include <functional>
#include "../types/object.h"
#include "../pimpl.h"

namespace REDasm {

class EventImpl;
class EventArgsImpl;

class EventArgs
{
    PIMPL_DECLARE_P(EventArgs)
    PIMPL_DECLARE_PRIVATE(EventArgs)

    public:
        EventArgs();
        EventArgs(const Variant& v);
        const Variant& arg() const;
        void* sender() const;
        Object* senderObject() const;

    friend class Event;
};

class Event: public Object
{
    REDASM_OBJECT(Event)
    PIMPL_DECLARE_P(Event)
    PIMPL_DECLARE_PRIVATE(Event)

    public:
        typedef std::function<void(EventArgs*)> HandlerType;

    public:
        Event();
        ~Event();
        void disconnect();
        void disconnect(void* owner);
        void connect(void* owner, const HandlerType& handler);
        void operator()() const;
        void operator()(const Variant& v) const;
        void operator()(EventArgs* e) const;

    public:
        Event(const Event& rhs) = delete;
        Event& operator =(const Event& rhs) = delete;
};

}
