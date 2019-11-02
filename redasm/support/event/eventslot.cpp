#include "eventslot.h"
#include <impl/support/event/eventslot_impl.h>

namespace REDasm {

EventSlot::EventSlot(): m_pimpl_p(new EventSlotImpl()) { }
EventSlot::EventSlot(EventSlot::Callable&& c): m_pimpl_p(new EventSlotImpl(std::move(c))) { }
EventSlot::Id EventSlot::id() const { PIMPL_P(const EventSlot); return p->m_id; }
void EventSlot::operator()(const EventArgs* e) const { PIMPL_P(const EventSlot); p->m_callable(e); }
bool EventSlot::operator==(const EventSlot& rhs) const { PIMPL_P(const EventSlot); return p->m_id == rhs.id(); }
bool EventSlot::operator!=(const EventSlot& rhs) const { PIMPL_P(const EventSlot); return p->m_id != rhs.id(); }

} // namespace REDasm
