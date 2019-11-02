#pragma once

#include <redasm/support/event/eventslot.h>

namespace REDasm {

class EventSlotImpl
{
    public:
        EventSlotImpl();
        EventSlotImpl(EventSlot::Callable&& c);

    public:
        EventSlot::Callable m_callable;
        EventSlot::Id m_id;

    private:
        static EventSlot::Id m_slotid;
};

} // namespace REDasm

