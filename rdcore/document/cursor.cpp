#include "cursor.h"
#include "../context.h"
#include <rdcore/eventdispatcher.h>
#include <rdcore/document/document.h>
#include <tuple>

Cursor::Cursor(SafeDocument& d): m_document(d) { }
void Cursor::toggle() { m_active = !m_active; }
void Cursor::enable() { m_active = true; }
void Cursor::disable() { m_active = false; }

void Cursor::goBack()
{
    if(m_backstack.empty()) return;

    RDCursorPos pos = m_backstack.top();
    m_backstack.pop();
    m_forwardstack.push(m_position);
    this->moveTo(pos.line, pos.column, false);
    m_document->context()->enqueue<RDCursorEventArgs>(Event_CursorStackChanged, this, &m_position, &m_selection);
}

void Cursor::goForward()
{
    if(m_forwardstack.empty()) return;

    RDCursorPos pos = m_forwardstack.top();
    m_forwardstack.pop();
    m_backstack.push(m_position);
    this->moveTo(pos.line, pos.column, false);
    m_document->context()->enqueue<RDCursorEventArgs>(Event_CursorStackChanged, this, &m_position, &m_selection);
}

void Cursor::clearSelection()
{
    if(Cursor::equalPos(&m_position, &m_selection)) return;

    m_selection = m_position;
    m_document->context()->enqueue<RDCursorEventArgs>(Event_CursorPositionChanged, this, &m_position, &m_selection);
}

void Cursor::set(size_t line, size_t column) { this->moveTo(line, column, false); }
void Cursor::moveTo(size_t line, size_t column) { this->moveTo(line, column, true); }

void Cursor::select(size_t line, size_t column)
{
    m_position = { std::max<size_t>(line, 0),
                   std::max<size_t>(column, 0) };

    m_document->context()->enqueue<RDCursorEventArgs>(Event_CursorPositionChanged, this, &m_position, &m_selection);
}

const RDCursorPos* Cursor::position() const { return &m_position; }
const RDCursorPos* Cursor::selection() const { return &m_selection; }

const RDCursorPos* Cursor::startSelection() const
{
    if(m_position.line < m_selection.line)
        return &m_position;

    if(m_position.line == m_selection.line)
    {
        if(m_position.column < m_selection.column)
            return &m_position;
    }

    return &m_selection;
}

const RDCursorPos* Cursor::endSelection() const
{
    if(m_position.line > m_selection.line)
        return &m_position;

    if(m_position.line == m_selection.line)
    {
        if(m_position.column > m_selection.column)
            return &m_position;
    }

    return &m_selection;
}

size_t Cursor::currentLine() const { return m_position.line; }
size_t Cursor::currentColumn() const { return m_position.column; }
size_t Cursor::selectionLine() const { return m_selection.line; }
size_t Cursor::selectionColumn() const { return m_selection.column; }

bool Cursor::isLineSelected(size_t line) const
{
    if(!this->hasSelection()) return false;

    size_t first = std::min(m_position.line, m_selection.line);
    size_t last = std::max(m_position.line, m_selection.line);

    if((line < first) || (line > last)) return false;
    return true;
}

bool Cursor::hasSelection() const { return !Cursor::equalPos(&m_position, &m_selection); }
bool Cursor::canGoBack() const { return !m_backstack.empty(); }
bool Cursor::canGoForward() const { return !m_forwardstack.empty(); }
bool Cursor::active() const { return m_active; }

bool Cursor::equalPos(const RDCursorPos* pos1, const RDCursorPos* pos2)
{
    return std::tie(pos1->line, pos1->column) ==
           std::tie(pos2->line, pos2->column);
}

void Cursor::moveTo(size_t line, size_t column, bool save)
{
    RDCursorPos pos = { std::max<size_t>(line, 0),
                        std::max<size_t>(column, 0) };

    if(save && !this->hasSelection())
    {
        if(m_backstack.empty() || (!m_backstack.empty() && !Cursor::equalPos(&m_backstack.top(), &m_position)))
        {
            m_backstack.push(m_position);
            m_document->context()->enqueue<RDCursorEventArgs>(Event_CursorStackChanged, this, &m_position, &m_selection);
        }
    }

    m_selection = pos;
    this->select(line, column);
}
