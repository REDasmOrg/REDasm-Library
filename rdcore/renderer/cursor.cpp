#include "cursor.h"
#include "../context.h"

Cursor::Cursor(Context* ctx): Object(ctx) { }

void Cursor::goBack()
{
    if(m_backstack.empty()) return;

    RDSurfacePos pos = m_backstack.top();
    m_backstack.pop();
    m_forwardstack.push(m_position);
    this->moveTo(pos.row, pos.col, false, true);
    this->onStackChanged();
}

void Cursor::goForward()
{
    if(m_forwardstack.empty()) return;

    RDSurfacePos pos = m_forwardstack.top();
    m_forwardstack.pop();
    m_backstack.push(m_position);
    this->moveTo(pos.row, pos.col, false, true);
    this->onStackChanged();
}

void Cursor::clearSelection()
{
    if(Cursor::equalPos(&m_position, &m_selection)) return;

    m_selection = m_position;
    this->onPositionChanged();
}

void Cursor::moveTo(int row, int col) { this->moveTo(row, col, true, true); }

void Cursor::select(int row, int col)
{
    m_position = { row, col };
    this->onPositionChanged();
}

const RDSurfacePos* Cursor::position() const { return &m_position; }
const RDSurfacePos* Cursor::selection() const { return &m_selection; }

const RDSurfacePos* Cursor::startSelection() const
{
    if(m_position.row < m_selection.row) return &m_position;

    if(m_position.row == m_selection.row)
    {
        if(m_position.col < m_selection.col)
            return &m_position;
    }

    return &m_selection;
}

const RDSurfacePos* Cursor::endSelection() const
{
    if(m_position.row > m_selection.row) return &m_position;

    if(m_position.row == m_selection.row)
    {
        if(m_position.col > m_selection.col)
            return &m_position;
    }

    return &m_selection;
}

int Cursor::currentRow() const { return m_position.row; }
int Cursor::currentColumn() const { return m_position.col; }
int Cursor::selectionLine() const { return m_selection.row; }
int Cursor::selectionColumn() const { return m_selection.col; }

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

void Cursor::set(int row, int col) { this->moveTo(row, col, true, false); }

bool Cursor::equalPos(const RDSurfacePos* pos1, const RDSurfacePos* pos2)
{
    return std::tie(pos1->row, pos1->col) ==
           std::tie(pos2->row, pos2->col);
}

void Cursor::moveTo(int row, int column, bool save, bool notify)
{
    RDSurfacePos pos = { row, column };

    if(save && !this->hasSelection())
    {
        if(m_backstack.empty() || (!m_backstack.empty() && !Cursor::equalPos(&m_backstack.top(), &m_position)))
        {
            m_backstack.push(m_position);
            this->onStackChanged();
        }
    }

    m_selection = pos;
    this->select(row, column, notify);
}

void Cursor::select(int row, int col, bool notify)
{
    m_position = { row, col };
    if(notify) this->onPositionChanged();
}
