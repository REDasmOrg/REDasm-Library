#include "eventmanager_impl.h"
#include <algorithm>

namespace REDasm {

std::recursive_mutex EventManagerImpl::m_mutex;
EventManagerImpl::Subscribers EventManagerImpl::m_subscribers;
EventManagerImpl::Groups EventManagerImpl::m_groups;

void EventManagerImpl::unsubscribe(EventSlot::Id slotid)
{
    auto sit = std::find_if(m_subscribers.begin(), m_subscribers.end(), [slotid](const Subscribers::value_type& item) -> bool {
        return slotid == item.second.id();
    });

    if(sit != m_subscribers.end()) m_subscribers.erase(sit);
}

} // namespace REDasm
