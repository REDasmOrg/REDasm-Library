#include "cursor.h"
#include "../document/backend/itemcontainer.h"
#include "../context.h"
#include "surface.h"

CursorHistory::CursorHistory(const CursorHistory::History& backstack, const CursorHistory::History& forwardstack): m_hback(backstack), m_hforward(forwardstack) { }
CursorHistory::History& CursorHistory::backStack() { return m_hback; }
CursorHistory::History& CursorHistory::forwardStack() { return m_hforward; }
bool CursorHistory::canGoForward() const { return !m_hforward.empty(); }
bool CursorHistory::canGoBack() const { return !m_hback.empty(); }

Cursor::Cursor(Context* ctx): Object(ctx) { m_history = std::make_shared<CursorHistory>(); }
bool Cursor::hasSelection() const { return !Cursor::equalPos(&m_position, &m_selection); }
bool Cursor::canGoForward() const { return m_history->canGoForward(); }
bool Cursor::canGoBack() const { return m_history->canGoBack(); }
int Cursor::currentRow() const { return m_position.row; }
int Cursor::currentColumn() const { return m_position.col; }

void Cursor::clearSelection()
{
    if(Cursor::equalPos(&m_position, &m_selection)) return;
    m_selection = m_position;
}

void Cursor::goForward()
{
    if(!m_history->canGoForward()) return;

    auto item = m_history->forwardStack().top();
    m_history->forwardStack().pop();
    if(m_currentitem.type) m_history->backStack().push(m_currentitem);

    this->moveSurfaces(&item);
    this->notifyHistoryChanged();
}

void Cursor::goBack()
{
    if(!m_history->canGoBack()) return;

    auto item = m_history->backStack().top();
    m_history->backStack().pop();
    if(m_currentitem.type) m_history->forwardStack().push(m_currentitem);

    this->moveSurfaces(&item);
    this->notifyHistoryChanged();
}

void Cursor::setCurrentItem(const RDDocumentItem& item) { m_currentitem = item; }
void Cursor::set(int row, int col) { this->moveTo(row, col, false); }
void Cursor::moveTo(int row, int col) { this->moveTo(row, col, true); }
void Cursor::select(int row, int col) { this->select(row, col, true); }

void Cursor::updateHistory()
{
    if(!m_currentitem.type) return;
    if(!m_history->backStack().empty() && ItemContainer::equals(&m_currentitem, &m_history->backStack().top())) return;

    m_history->backStack().push(m_currentitem);
    this->notifyHistoryChanged();
}

void Cursor::attach(Surface* s) { m_surfaces.insert(s); }
void Cursor::detach(Surface* s) { m_surfaces.erase(s); }
void Cursor::moveSurfaces(const RDDocumentItem* item) { for(Surface* s : m_surfaces) s->goTo(item, false); }
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
const CursorHistoryPtr& Cursor::history() const { return m_history; }

void Cursor::linkHistory(const CursorHistoryPtr& ptr)
{
    if(m_history == ptr) return;

    bool notify = m_history != nullptr;
    m_history = ptr;

    if(!m_history) m_history = std::make_shared<CursorHistory>();
    if(notify) this->notifyHistoryChanged();
}

void Cursor::unlinkHistory()
{
    m_history = std::make_shared<CursorHistory>(m_history->backStack(), m_history->forwardStack());
    this->notifyHistoryChanged();
}

bool Cursor::equalPos(const RDSurfacePos* pos1, const RDSurfacePos* pos2) { return std::tie(pos1->row, pos1->col) == std::tie(pos2->row, pos2->col); }
