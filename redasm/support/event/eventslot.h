#pragma once

#include <functional>
#include "eventargs.h"
#include "../../pimpl.h"

namespace REDasm {

class EventSlotImpl;

class EventSlot
{
    PIMPL_DECLARE_P(EventSlot)
    PIMPL_DECLARE_PRIVATE(EventSlot)

    public:
        typedef size_t Id;
        typedef std::function<void(const EventArgs*)> Callable;

    public:
        EventSlot();
        EventSlot(Callable&& c);
        EventSlot(const EventSlot& rhs) = delete;
        Id id() const;
        void operator()(const EventArgs* e) const;
        bool operator==(const EventSlot& rhs) const;
        bool operator!=(const EventSlot& rhs) const;
};

} // namespace REDasm

