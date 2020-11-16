#include "cursor.h"
#include "../document/backend/itemcontainer.h"
#include "../context.h"
#include "surface.h"

Cursor::Cursor(Context* ctx): Object(ctx) { }
bool Cursor::hasSelection() const { return !Cursor::equalPos(&m_position, &m_selection); }
bool Cursor::canGoForward() const { return !m_hforward.empty(); }
bool Cursor::canGoBack() const { return !m_hback.empty(); }
int Cursor::currentRow() const { return m_position.row; }
int Cursor::currentColumn() const { return m_position.col; }

void Cursor::clearSelection()
{
    if(Cursor::equalPos(&m_position, &m_selection)) return;
    m_selection = m_position;
}

void Cursor::goForward()
{
    if(m_hforward.empty()) return;

    auto pos = m_hforward.top();
    m_hforward.pop();
    m_hback.push(m_position);
    this->moveTo(pos.row, pos.col);
    this->notifyHistoryChanged();
}

void Cursor::goBack()
{
    if(m_hback.empty()) return;

    auto pos = m_hback.top();
    m_hback.pop();
    m_hforward.push(m_position);
    this->moveTo(pos.row, pos.col);
    this->notifyHistoryChanged();
}

void Cursor::setCurrentItem(const RDDocumentItem& item) { m_currentitem = item; }
void Cursor::set(int row, int col) { this->moveTo(row, col, false); }
void Cursor::moveTo(int row, int col) { this->moveTo(row, col, true); }
void Cursor::select(int row, int col) { this->select(row, col, true); }
void Cursor::attach(Surface* s) { m_surfaces.insert(s); }
void Cursor::detach(Surface* s) { m_surfaces.erase(s); }
void Cursor::notifyHistoryChanged() { for(Surface* s : m_surfaces) s->notifyHistoryChanged(); }
void Cursor::notifyPositionChanged() { for(Surface* s : m_surfaces) s->notifyPositionChanged(); }

void Cursor::moveTo(int row, int col, bool notify)
{
    RDSurfacePos pos = { row, col };
    m_selection = pos;
    this->select(row, col, notify);
}

void Cursor::select(int row, int col, bool notify)
{
    m_position = { row, col };
    if(notify) this->notifyPositionChanged();
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

const RDDocumentItem& Cursor::currentItem() const { return m_currentitem; }
bool Cursor::equalPos(const RDSurfacePos* pos1, const RDSurfacePos* pos2) { return std::tie(pos1->row, pos1->col) == std::tie(pos2->row, pos2->col); }
