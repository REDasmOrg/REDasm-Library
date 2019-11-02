#include "eventslot_impl.h"

namespace REDasm {

EventSlot::Id EventSlotImpl::m_slotid = 0;

EventSlotImpl::EventSlotImpl()
{

}

EventSlotImpl::EventSlotImpl(EventSlot::Callable&& c): m_callable(c), m_id(++m_slotid) { }

} // namespace REDasm
