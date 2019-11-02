#pragma once

#include "eventslot.h"
#include "eventargs.h"
#include "standardevents.h"

namespace REDasm {

class EventManager
{
    public:
        typedef uintptr_t GroupId;

    public:
        EventManager() = delete;
        static EventSlot::Id subscribe(EventArgs::Id eventid, GroupId groupid, EventSlot::Callable&& slot);
        static EventSlot::Id subscribe(EventArgs::Id eventid, EventSlot::Callable&& slot);
        static void trigger(EventArgs::Id eventid, const EventArgs& e);
        static void trigger(EventArgs::Id eventid);
        static void unevent(EventArgs::Id eventid);
        static void unslot(EventSlot::Id slotid);
        static void ungroup(GroupId groupid);
        static void unsubscribe();

        template<EventArgs::Id eventid, typename T> static void trigger(const T& value) {
            EventManager::trigger(eventid, ValueEventArgs<T>(value));
        }

        template<typename Class> static void ungroup(Class* c) { EventManager::ungroup(reinterpret_cast<GroupId>(c)); }
        template<typename GID, typename Slot> static EventSlot::Id subscribe(EventArgs::Id eventid, GID* gid, Slot&& slot) {
            return EventManager::subscribe(eventid, reinterpret_cast<GroupId>(gid), slot);
        }
        template<typename Class, typename Member> static EventSlot::Id subscribe_m(EventArgs::Id eventid, Class* c, Member&& member) {
            return EventManager::subscribe(eventid, reinterpret_cast<GroupId>(c), std::bind(member, c, std::placeholders::_1));
        }
};

} // namespace REDasm

