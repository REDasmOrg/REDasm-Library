#include "listingcursor.h"
#include <impl/disassembler/listing/document/listingcursor_impl.h>
#include <redasm/support/event/eventmanager.h>

namespace REDasm {

void ListingCursor::save(cereal::BinaryOutputArchive &a) const { PIMPL_P(const ListingCursor); return p->save(a); }
void ListingCursor::load(cereal::BinaryInputArchive &a) { PIMPL_P(ListingCursor); return p->load(a); }
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
    EventManager::trigger(StandardEvents::Cursor_PositionChanged);
}

const ListingCursor::Position &ListingCursor::currentPosition() const { PIMPL_P(const ListingCursor); return p->m_position; }
const ListingCursor::Position &ListingCursor::currentSelection() const { PIMPL_P(const ListingCursor); return p->m_selection; }

const ListingCursor::Position &ListingCursor::startSelection() const
{
    PIMPL_P(const ListingCursor);

    if(p->m_position.line < p->m_selection.line)
        return p->m_position;

    if(p->m_position.line == p->m_selection.line)
    {
        if(p->m_position.column < p->m_selection.column)
            return p->m_position;
    }

    return p->m_selection;
}

const ListingCursor::Position &ListingCursor::endSelection() const
{
    PIMPL_P(const ListingCursor);

    if(p->m_position.line > p->m_selection.line)
        return p->m_position;

    if(p->m_position.line == p->m_selection.line)
    {
        if(p->m_position.column > p->m_selection.column)
            return p->m_position;
    }

    return p->m_selection;
}

size_t ListingCursor::currentLine() const { PIMPL_P(const ListingCursor); return p->m_position.line; }
size_t ListingCursor::currentColumn() const { PIMPL_P(const ListingCursor); return p->m_position.column; }
size_t ListingCursor::selectionLine() const { PIMPL_P(const ListingCursor); return p->m_selection.line; }
size_t ListingCursor::selectionColumn() const { PIMPL_P(const ListingCursor); return p->m_selection.column; }

bool ListingCursor::isLineSelected(size_t line) const
{
    if(!this->hasSelection())
        return false;

    PIMPL_P(const ListingCursor);
    size_t first = std::min(p->m_position.line, p->m_selection.line);
    size_t last = std::max(p->m_position.line, p->m_selection.line);

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

    p->m_position = { std::max<size_t>(line, 0),
                      std::max<size_t>(column, 0) };

    EventManager::trigger(StandardEvents::Cursor_PositionChanged);
}

void ListingCursor::goBack()
{
    PIMPL_P(ListingCursor);

    if(p->m_backstack.empty())
        return;

    Position pos = p->m_backstack.top();
    p->m_backstack.pop();

    p->m_forwardstack.push(p->m_position);
    p->moveTo(pos.line, pos.column, false);
    EventManager::trigger(StandardEvents::Cursor_BackChanged);
    EventManager::trigger(StandardEvents::Cursor_ForwardChanged);
}

void ListingCursor::goForward()
{
    PIMPL_P(ListingCursor);

    if(p->m_forwardstack.empty())
        return;

    Position pos = p->m_forwardstack.top();
    p->m_forwardstack.pop();

    p->m_backstack.push(p->m_position);
    p->moveTo(pos.line, pos.column, false);
    EventManager::trigger(StandardEvents::Cursor_BackChanged);
    EventManager::trigger(StandardEvents::Cursor_ForwardChanged);
}

bool ListingCursor::Position::operator ==(const ListingCursor::Position &rhs) const
{
    return (this->line == rhs.line) && (this->column == rhs.column);
}

bool ListingCursor::Position::operator !=(const ListingCursor::Position &rhs) const
{
    return (this->line != rhs.line) || (this->column != rhs.column);
}

} // namespace REDasm
