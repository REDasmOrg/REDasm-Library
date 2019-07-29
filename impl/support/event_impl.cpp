#include "event_impl.h"

namespace REDasm {

EventArgsImpl::EventArgsImpl(): m_sender(nullptr) { }
EventArgsImpl::EventArgsImpl(const Variant &arg): m_arg(arg), m_sender(nullptr) { }
const Variant& EventArgsImpl::arg() const { return m_arg; }
void *EventArgsImpl::sender() const { return m_sender; }
void EventArgsImpl::setSender(void *obj) { m_sender = obj; }

} // namespace REDasm
