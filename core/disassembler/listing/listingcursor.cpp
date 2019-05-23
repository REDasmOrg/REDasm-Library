#include "listingcursor.h"

namespace REDasm {

ListingCursor::ListingCursor(): m_active(false) { m_position = std::make_pair(0, 0); }
bool ListingCursor::active() const { return m_active; }
void ListingCursor::toggle() { m_active = !m_active; }
void ListingCursor::enable() { m_active = true; }
void ListingCursor::disable() { m_active = false; }

void ListingCursor::clearSelection()
{
    if(m_position == m_selection)
        return;

    m_selection = m_position;
    positionChanged();
}

const ListingCursor::Position &ListingCursor::currentPosition() const { return m_position; }
const ListingCursor::Position &ListingCursor::currentSelection() const { return m_selection; }

const ListingCursor::Position &ListingCursor::startSelection() const
{
    if(m_position.first < m_selection.first)
        return m_position;

    if(m_position.first == m_selection.first)
    {
        if(m_position.second < m_selection.second)
            return m_position;
    }

    return m_selection;
}

const ListingCursor::Position &ListingCursor::endSelection() const
{
    if(m_position.first > m_selection.first)
        return m_position;

    if(m_position.first == m_selection.first)
    {
        if(m_position.second > m_selection.second)
            return m_position;
    }

    return m_selection;
}

size_t ListingCursor::currentLine() const { return m_position.first; }
size_t ListingCursor::currentColumn() const { return m_position.second; }
size_t ListingCursor::selectionLine() const { return m_selection.first; }
size_t ListingCursor::selectionColumn() const { return m_selection.second; }

bool ListingCursor::isLineSelected(size_t line) const
{
    if(!this->hasSelection())
        return false;

    size_t first = std::min(m_position.first, m_selection.first);
    size_t last = std::max(m_position.first, m_selection.first);

    if((line < first) || (line > last))
        return false;

    return true;
}

bool ListingCursor::hasSelection() const { return m_position != m_selection; }
bool ListingCursor::canGoBack() const { return !m_backstack.empty(); }
bool ListingCursor::canGoForward() const { return !m_forwardstack.empty(); }
void ListingCursor::set(size_t line, size_t column) { this->moveTo(line, column, false); }
void ListingCursor::moveTo(size_t line, size_t column) { this->moveTo(line, column, true); }

void ListingCursor::select(size_t line, size_t column)
{
    m_position = std::make_pair(std::max(line, static_cast<size_t>(0)),
                                std::max(column, static_cast<size_t>(0)));

    positionChanged();
}

void ListingCursor::goBack()
{
    if(m_backstack.empty())
        return;

    Position pos = m_backstack.top();
    m_backstack.pop();

    m_forwardstack.push(m_position);
    this->moveTo(pos.first, pos.second, false);
    backChanged();
    forwardChanged();
}

void ListingCursor::goForward()
{
    if(m_forwardstack.empty())
        return;

    Position pos = m_forwardstack.top();
    m_forwardstack.pop();

    m_backstack.push(m_position);
    this->moveTo(pos.first, pos.second, false);
    backChanged();
    forwardChanged();
}

void ListingCursor::moveTo(size_t line, size_t column, bool save)
{
    Position pos = std::make_pair(std::max(line, size_t(0)),
                                  std::max(column, size_t(0)));

    if(save && !this->hasSelection())
    {
        if(m_backstack.empty() || (!m_backstack.empty() && (m_backstack.top() != m_position)))
        {
            m_backstack.push(m_position);
            backChanged();
        }
    }

    m_selection = pos;
    this->select(line, column);
}

} // namespace REDasm
