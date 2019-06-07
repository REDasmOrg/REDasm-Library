#include "listingcursor_impl.h"

namespace REDasm {

ListingCursorImpl::ListingCursorImpl(ListingCursor* q): m_pimpl_q(q), m_active(false) { m_position = std::make_pair(0, 0); }

void ListingCursorImpl::moveTo(size_t line, size_t column, bool save)
{
    PIMPL_Q(ListingCursor);

    ListingCursor::Position pos = std::make_pair(std::max(line, size_t(0)),
                                  std::max(column, size_t(0)));

    if(save && !q->hasSelection())
    {
        if(m_backstack.empty() || (!m_backstack.empty() && (m_backstack.top() != m_position)))
        {
            m_backstack.push(m_position);
            q->backChanged();
        }
    }

    m_selection = pos;
    q->select(line, column);
}

} // namespace REDasm
