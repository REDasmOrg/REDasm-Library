#include "listingcursor.h"
#include <impl/disassembler/listing/listingcursor_impl.h>

namespace REDasm {

ListingCursor::ListingCursor(): m_pimpl_p(new ListingCursorImpl(this)) { }
bool ListingCursor::active() const { PIMPL_P(const ListingCursor); return p->m_active; }
void ListingCursor::toggle() { PIMPL_P(ListingCursor); p->m_active = !p->m_active; }
void ListingCursor::enable() { PIMPL_P(ListingCursor); p->m_active = true; }
void ListingCursor::disable() { PIMPL_P(ListingCursor); p->m_active = false; }

void ListingCursor::clearSelection()
{
    PIMPL_P(ListingCursor);

    if(p->m_position == p->m_selection)
        return;

    p->m_selection = p->m_position;
    positionChanged();
}

const ListingCursor::Position &ListingCursor::currentPosition() const { PIMPL_P(const ListingCursor); return p->m_position; }
const ListingCursor::Position &ListingCursor::currentSelection() const { PIMPL_P(const ListingCursor); return p->m_selection; }

const ListingCursor::Position &ListingCursor::startSelection() const
{
    PIMPL_P(const ListingCursor);

    if(p->m_position.first < p->m_selection.first)
        return p->m_position;

    if(p->m_position.first == p->m_selection.first)
    {
        if(p->m_position.second < p->m_selection.second)
            return p->m_position;
    }

    return p->m_selection;
}

const ListingCursor::Position &ListingCursor::endSelection() const
{
    PIMPL_P(const ListingCursor);

    if(p->m_position.first > p->m_selection.first)
        return p->m_position;

    if(p->m_position.first == p->m_selection.first)
    {
        if(p->m_position.second > p->m_selection.second)
            return p->m_position;
    }

    return p->m_selection;
}

size_t ListingCursor::currentLine() const { PIMPL_P(const ListingCursor); return p->m_position.first; }
size_t ListingCursor::currentColumn() const { PIMPL_P(const ListingCursor); return p->m_position.second; }
size_t ListingCursor::selectionLine() const { PIMPL_P(const ListingCursor); return p->m_selection.first; }
size_t ListingCursor::selectionColumn() const { PIMPL_P(const ListingCursor); return p->m_selection.second; }

bool ListingCursor::isLineSelected(size_t line) const
{
    if(!this->hasSelection())
        return false;

    PIMPL_P(const ListingCursor);
    size_t first = std::min(p->m_position.first, p->m_selection.first);
    size_t last = std::max(p->m_position.first, p->m_selection.first);

    if((line < first) || (line > last))
        return false;

    return true;
}

bool ListingCursor::hasSelection() const { PIMPL_P(const ListingCursor); return p->m_position != p->m_selection; }
bool ListingCursor::canGoBack() const { PIMPL_P(const ListingCursor); return !p->m_backstack.empty(); }
bool ListingCursor::canGoForward() const { PIMPL_P(const ListingCursor); return !p->m_forwardstack.empty(); }
void ListingCursor::set(size_t line, size_t column) { PIMPL_P(ListingCursor); p->moveTo(line, column, false); }
void ListingCursor::moveTo(size_t line, size_t column) { PIMPL_P(ListingCursor); p->moveTo(line, column, true); }

void ListingCursor::select(size_t line, size_t column)
{
    PIMPL_P(ListingCursor);
    p->m_position = std::make_pair(std::max(line, static_cast<size_t>(0)),
                                std::max(column, static_cast<size_t>(0)));

    positionChanged();
}

void ListingCursor::goBack()
{
    PIMPL_P(ListingCursor);

    if(p->m_backstack.empty())
        return;

    Position pos = p->m_backstack.top();
    p->m_backstack.pop();

    p->m_forwardstack.push(p->m_position);
    p->moveTo(pos.first, pos.second, false);
    backChanged();
    forwardChanged();
}

void ListingCursor::goForward()
{
    PIMPL_P(ListingCursor);

    if(p->m_forwardstack.empty())
        return;

    Position pos = p->m_forwardstack.top();
    p->m_forwardstack.pop();

    p->m_backstack.push(p->m_position);
    p->moveTo(pos.first, pos.second, false);
    backChanged();
    forwardChanged();
}

} // namespace REDasm
