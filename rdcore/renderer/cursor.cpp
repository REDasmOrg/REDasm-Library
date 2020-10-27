#include "cursor.h"
#include "../context.h"

Cursor::Cursor(Context* ctx): Object(ctx) { }
void Cursor::toggle()
{
    m_active = !m_active;
    this->onCursorChanged();
}

void Cursor::enable()
{
    if(m_active) return;
    m_active = true;
    this->onCursorChanged();
}

void Cursor::disable()
{
    if(!m_active) return;
    m_active = false;
    this->onCursorChanged();
}

void Cursor::goBack()
{
    if(m_backstack.empty()) return;

    RDCursorPos pos = m_backstack.top();
    m_backstack.pop();
    m_forwardstack.push(m_position);
    this->moveTo(pos.row, pos.column, false);
    this->notify<RDCursorEventArgs>(Event_CursorStackChanged, this, &m_position, &m_selection);
}

void Cursor::goForward()
{
    if(m_forwardstack.empty()) return;

    RDCursorPos pos = m_forwardstack.top();
    m_forwardstack.pop();
    m_backstack.push(m_position);
    this->moveTo(pos.row, pos.column, false);
    this->notify<RDCursorEventArgs>(Event_CursorStackChanged, this, &m_position, &m_selection);
}

void Cursor::clearSelection()
{
    if(Cursor::equalPos(&m_position, &m_selection)) return;

    m_selection = m_position;
    this->onCursorChanged();
}

void Cursor::set(int row, int col) { this->moveTo(row, col, false); }
void Cursor::moveTo(int row, int col) { this->moveTo(row, col, true); }

void Cursor::select(int row, int col)
{
    m_position = { row, col };
    this->onCursorChanged();
}

const RDCursorPos* Cursor::position() const { return &m_position; }
const RDCursorPos* Cursor::selection() const { return &m_selection; }

const RDCursorPos* Cursor::startSelection() const
{
    if(m_position.row < m_selection.row) return &m_position;

    if(m_position.row == m_selection.row)
    {
        if(m_position.column < m_selection.column)
            return &m_position;
    }

    return &m_selection;
}

const RDCursorPos* Cursor::endSelection() const
{
    if(m_position.row > m_selection.row) return &m_position;

    if(m_position.row == m_selection.row)
    {
        if(m_position.column > m_selection.column)
            return &m_position;
    }

    return &m_selection;
}

int Cursor::currentRow() const { return m_position.row; }
int Cursor::currentColumn() const { return m_position.column; }
int Cursor::selectionLine() const { return m_selection.row; }
int Cursor::selectionColumn() const { return m_selection.column; }

bool Cursor::isRowSelected(int row) const
{
    if(!this->hasSelection()) return false;

    int first = std::min(m_position.row, m_selection.row);
    int last = std::max(m_position.row, m_selection.row);

    if((row < first) || (row > last)) return false;
    return true;
}

bool Cursor::hasSelection() const { return !Cursor::equalPos(&m_position, &m_selection); }
bool Cursor::canGoBack() const { return !m_backstack.empty(); }
bool Cursor::canGoForward() const { return !m_forwardstack.empty(); }
bool Cursor::active() const { return m_active; }
void Cursor::onCursorChanged() { }

bool Cursor::equalPos(const RDCursorPos* pos1, const RDCursorPos* pos2)
{
    return std::tie(pos1->row, pos1->column) ==
           std::tie(pos2->row, pos2->column);
}

void Cursor::moveTo(int row, int column, bool save)
{
    RDCursorPos pos = { row, column };

    if(save && !this->hasSelection())
    {
        if(m_backstack.empty() || (!m_backstack.empty() && !Cursor::equalPos(&m_backstack.top(), &m_position)))
        {
            m_backstack.push(m_position);
            this->notify<RDCursorEventArgs>(Event_CursorStackChanged, this, &m_position, &m_selection);
        }
    }

    m_selection = pos;
    this->select(row, column);
}
