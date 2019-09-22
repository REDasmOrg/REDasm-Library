#include "event.h"
#include <algorithm>
#include <impl/support/event_impl.h>

namespace REDasm {

EventArgs::EventArgs(EventArgsImpl* e): m_pimpl_p(e) { }
EventArgs::EventArgs(): m_pimpl_p(new EventArgsImpl()) { }
EventArgs::EventArgs(const Variant &v): m_pimpl_p(new EventArgsImpl(v)) { }
const Variant &EventArgs::arg() const { PIMPL_P(const EventArgs); return p->arg(); }
void *EventArgs::sender() const { PIMPL_P(const EventArgs); return p->sender(); }
Object *EventArgs::senderObject() const { return reinterpret_cast<Object*>(this->sender()); }

Event::Event(): m_pimpl_p(new EventImpl()) { }
Event::~Event() { this->disconnect(); }
void Event::connect(void *owner, const Event::HandlerType &handler) { PIMPL_P(Event); p->m_handlers.emplace_back(owner, handler); }
void Event::disconnect() { PIMPL_P(Event); p->m_handlers.clear(); }
void Event::operator()() const { EventArgs e; (*this)(e); }
void Event::operator()(const Variant &v) const { EventArgs e(v); (*this)(e); }

void Event::operator()(EventArgs &e) const
{
    PIMPL_P(const Event);

    std::for_each(p->m_handlers.begin(), p->m_handlers.end(), [&](const EventImpl::HandlerItem& item) {
        EventArgs ee = e; // Copy event
        ee.pimpl_p()->setSender(item.first);
        item.second(&ee);
    });
}

void Event::disconnect(void *owner)
{
    PIMPL_P(Event);
    auto it = p->m_handlers.begin();

    while(it != p->m_handlers.end())
    {
        if(it->first != owner)
            it++;
        else
            it = p->m_handlers.erase(it);
    }
}

} // namespace REDasm
