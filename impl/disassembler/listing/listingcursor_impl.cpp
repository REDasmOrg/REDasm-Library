#include "listingcursor_impl.h"
#include "../../libs/cereal/cereal.hpp"
#include "../../libs/cereal/types/stack.hpp"
#include "../../libs/cereal/archives/binary.hpp"
#include <redasm/support/event/eventmanager.h>

namespace REDasm {

ListingCursorImpl::ListingCursorImpl(ListingCursor* q): m_pimpl_q(q) { }

void ListingCursorImpl::save(cereal::BinaryOutputArchive &a) const
{
    a(m_position.line, m_position.column,
      m_selection.line, m_selection.column);
}

void ListingCursorImpl::load(cereal::BinaryInputArchive &a)
{
    a(m_position.line, m_position.column,
      m_selection.line, m_selection.column);
}

void ListingCursorImpl::moveTo(size_t line, size_t column, bool save)
{
    PIMPL_Q(ListingCursor);

    ListingCursor::Position pos = { std::max(line, size_t(0)),
                                    std::max(column, size_t(0)) };

    if(save && !q->hasSelection())
    {
        if(m_backstack.empty() || (!m_backstack.empty() && (m_backstack.top() != m_position)))
        {
            m_backstack.push(m_position);
            EventManager::trigger(StandardEvents::Cursor_BackChanged);
        }
    }

    m_selection = pos;
    q->select(line, column);
}

} // namespace REDasm
