#pragma once

#include <redasm/support/event.h>
#include <list>

namespace REDasm {

class EventArgsImpl
{
    public:
        EventArgsImpl();
        EventArgsImpl(const Variant& arg);
        const Variant& arg() const;
        void* sender() const;
        void setSender(void* obj);

    private:
        Variant m_arg;
        void* m_sender;
};

class EventImpl
{
    PIMPL_DECLARE_Q(Event)
    PIMPL_DECLARE_PUBLIC(Event)

    public:
        typedef std::pair<void*, Event::HandlerType> HandlerItem;

    public:
        EventImpl() = default;

    private:
        std::list<HandlerItem> m_handlers;
};

} // namespace REDasm
