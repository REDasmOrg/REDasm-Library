#include "surface.h"
#include "../document/document.h"
#include "../context.h"
#include "renderer.h"
#include <cmath>

Surface::Surface(Context* ctx, rd_flag flags): Cursor(ctx), m_path(this), m_flags(flags)
{
    this->context()->subscribe(this, std::bind(&Surface::handleEvents, this, std::placeholders::_1));

    RDDocumentItem item;

    {
        auto lock = s_lock_safe_ptr(this->document());
        const auto* items = lock->items();

        if(items->empty()) return;
        item = *items->begin();
    }

    this->goTo(&item);
}

Surface::~Surface() { this->document()->unsubscribe(this); }
size_t Surface::getPath(const RDPathItem** path) const { return m_path.getPath(path); }
int Surface::lastColumn() const { return m_lastcolumn; }

int Surface::row(int row, RDSurfaceCell* cells) const
{
    auto it = m_surfacerows.find(row);
    if(it == m_surfacerows.end()) return 0;

    SurfaceLock lock(m_mutex);
    if(cells) std::copy_n(std::addressof(m_surface[row * m_cols]), m_cols, cells);
    return m_cols;
}

bool Surface::currentItem(RDDocumentItem* item) const
{
    auto it = m_surfacerows.find(this->currentRow());
    if(it == m_surfacerows.end()) return false;

    if(item) *item = it->second.item;
    return true;
}

const RDDocumentItem* Surface::firstItem() const { return &m_items.first; }
const RDDocumentItem* Surface::lastItem() const { return &m_items.second; }

int Surface::findRow(const RDDocumentItem* item) const
{
    for(const auto& ri : m_surfacerows)
    {
        if(!ItemContainer::equals(&ri.second.item, item)) continue;
        return ri.first;
    }

    return -1;
}

bool Surface::getItem(int row, RDDocumentItem* item) const
{
    auto it = m_surfacerows.find(row);
    if(it == m_surfacerows.end()) return false;

    if(item) *item = it->second.item;
    return true;
}

bool Surface::currentSymbol(RDSymbol* symbol) const
{
    auto* cw = this->currentWord();
    if(!cw) return false;
    return this->document()->symbol(cw->c_str(), symbol);
}

bool Surface::symbolAt(int row, int col, RDSymbol* symbol) const
{
    auto* w = this->wordAt(row, col);
    if(!w) return false;
    return this->document()->symbol(w->c_str(), symbol);
}

const std::string* Surface::currentWord() const { return this->wordAt(this->currentRow(), this->currentColumn()); }

const std::string* Surface::wordAt(int row, int col) const
{
    auto it = m_surfacerows.find(row);
    if(it == m_surfacerows.end()) return nullptr;

    int currcol = 0;

    for(const std::string& c : it->second.chunks)
    {
        if((col >= currcol) && (col < static_cast<int>(currcol + c.size())))
            return std::addressof(c);

        currcol += c.size();
    }

    return nullptr;
}

bool Surface::goTo(const RDDocumentItem* item)
{
    if(!item) return false;

    auto it = this->items()->find(*item);
    if(it == this->items()->end()) return false;

    m_items.first = *it;
    this->update();
    return true;
}

bool Surface::goToAddress(rd_address address)
{
    auto it = this->items()->find(address);
    if(it == this->items()->end()) return false;
    return this->goTo(std::addressof(*it));
}

void Surface::getSize(int* rows, int* cols) const
{
    if(rows) *rows = m_rows;
    if(cols) *cols = m_cols;
}

void Surface::scroll(int nrows, int ncols)
{
    this->scrollRows(nrows);
    this->update();

    if(nrows) this->notifyChanged();
}

void Surface::resize(int rows, int cols)
{
    int area = rows * cols;
    if(area <= 0) return;

    {
        SurfaceLock lock(m_mutex);
        m_surface.resize(area, { });
    }

    m_rows = rows;
    m_cols = cols;
    this->update();
}

void Surface::moveTo(int row, int col)
{
    this->checkColumn(row, col);

    if(row < 0) this->scroll(row, 0);
    if(row >= m_rows) this->scroll((row - m_rows) + 1, 0);
    else Cursor::moveTo(row, col);
}

void Surface::select(int row, int col)
{
    if(row < 0) return;

    row = std::min(row, m_rows - 1);
    this->checkColumn(row, col);
    Cursor::select(row, col);
}

void Surface::onCursorChanged()
{
    this->update();
    this->notifyChanged();
}

const Surface::SurfaceRow* Surface::currentSurfaceRow() const
{
    auto it = m_surfacerows.find(this->currentRow());
    return (it != m_surfacerows.end()) ? std::addressof(it->second) : nullptr;
}

RDSurfaceCell* Surface::cell(size_t row, size_t col) { return std::addressof(m_surface[row * m_cols + col]); }
SafeDocument& Surface::document() const { return this->context()->document(); }
const ItemContainer* Surface::items() const { return this->context()->document()->items(); }

void Surface::notifyChanged()
{
    RDDocumentItem item;
    if(!this->currentItem(&item)) return;
    this->notify<RDSurfaceEventArgs>(Event_SurfaceChanged, this, this->position(), this->selection(), item);
}

void Surface::checkColumn(int row, int& column) const
{
    if(column == -1) column = m_cols - 1;
    else column = std::min(column, m_cols);

    auto it = m_surfacerows.find(row);
    if(it != m_surfacerows.end()) column = std::min(column, it->second.length - 1);
}

void Surface::scrollRows(int nrows)
{
    if(!nrows) return;
    this->clearSelection();

    auto it = this->items()->find(m_items.first);
    if(it == this->items()->end()) return;

    RDDocumentItem item = *it;

    for(int i = 0; i < std::abs(nrows); i++)
    {
        if(nrows > 0)
        {
            if(++it == this->items()->end()) break;
            item = *it;
        }
        else
        {
            if(it == this->items()->begin()) break;
            item = *(--it);
        }
    }

    m_items.first = item;
}

bool Surface::contains(const RDDocumentItem* item) const
{
    if(!item || !m_items.first.type || m_items.second.type) return false;
    return (item->address >= m_items.first.address) && (item->address <= m_items.second.address);
}

void Surface::handleEvents(const RDEventArgs* event)
{
    switch(event->eventid)
    {
        case Event_DocumentChanged:
        {
            auto* e = reinterpret_cast<const RDDocumentEventArgs*>(event);

            switch(e->action) {
                case DocumentAction_ItemChanged: {
                    if(!this->contains(&e->item)) return;
                    break;
                }

                case DocumentAction_ItemInserted:
                case DocumentAction_ItemRemoved: {
                    if(!m_items.second.type) return;
                    if(e->item.address > m_items.second.address) return;
                    break;
                }

                default: return;
            }

            break;
        }

        case Event_BusyChanged: {
            if(this->context()->busy()) return;
            RDLocation loc = this->document()->entryPoint();
            if(loc.valid) this->goToAddress(loc.address);
            break;
        }

        case Event_ContextFlagsChanged: break;
        default: return;
    }

    this->update();
}

void Surface::onCursorStackChanged() { this->notify<RDEventArgs>(Event_SurfaceCursorChanged, this); }

void Surface::update()
{
    if(!m_items.first.type || !m_rows || !m_cols) return;

    auto lock = s_lock_safe_ptr(this->document());
    const auto* items = lock->items();
    auto it = items->find(m_items.first);

    // Clear Cached Data
    std::fill(m_surface.begin(), m_surface.end(), RDSurfaceCell{ Theme_Default, Theme_Default, ' ' });
    m_surfacerows.clear();
    m_lastcolumn = 0;

    for(int row = 0; (row < m_rows) && (it != items->end()); row++, it++)
    {
        Renderer r(m_context, m_flags, &m_commentcolumn);
        if(!r.render(std::addressof(*it))) continue;

        m_surfacerows[row].item = *it;
        this->drawRow(row, r, m_surfacerows[row]);

        m_lastcolumn = std::max<int>(m_lastcolumn, r.text().size());
        m_items.second = *it;
    }

    this->highlightCurrentRow();
    if(!this->hasFlag(RendererFlags_NoHighlightWords)) this->highlightWords();

    this->highlightSelection();
    if(!this->hasFlag(RendererFlags_NoCursor)) this->drawCursor();

    if(!m_context->busy()) m_path.update();
    this->notify<RDEventArgs>(Event_SurfaceUpdated, this);
}

void Surface::drawRow(int row, const Renderer& st, SurfaceRow& sfrow)
{
    int col = 0;

    for(const auto& c : st.chunks())
    {
        sfrow.chunks.push_back(c.chunk);
        sfrow.length += c.chunk.size();

        for(const auto& ch : c.chunk)
        {
            if(col >= m_cols) return;
            *this->cell(row, col) = { c.background, c.foreground, ch };
            col++;
        }
    }
}

void Surface::drawCursor()
{
    if(!this->active()) return;
    if(this->currentRow() >= m_rows) return;
    if(this->currentColumn() >= m_cols) return;

    auto* cell = this->cell(this->currentRow(), this->currentColumn());
    cell->background = Theme_CursorBg;
    cell->foreground = Theme_CursorFg;
}

void Surface::highlightCurrentRow()
{
    if(this->hasFlag(RendererFlags_NoCursor) || (this->currentRow() >= m_rows)) return;

    for(int i = 0; i < m_cols; i++)
        this->cell(this->currentRow(), i)->background = Theme_Seek;
}

void Surface::highlightWords()
{
    if(!this->active() || this->hasSelection()) return;

    auto* cw = this->currentWord();
    if(!cw) return;

    for(const auto& [row, surfacerow] : m_surfacerows)
    {
        int col = 0;

        for(const auto& c : surfacerow.chunks)
        {
            for(int i = 0; (*cw == c) && (i < static_cast<int>(c.size())); i++)
            {
                if((col + i) >= m_cols) break;
                this->cell(row, col + i)->background = Theme_HighlightBg;
                this->cell(row, col + i)->foreground = Theme_HighlightFg;
            }

            col += c.size();
        }
    }
}

void Surface::highlightSelection()
{
    if(!this->hasSelection()) return;
    const RDSurfacePos* startsel = this->startSelection();
    const RDSurfacePos* endsel = this->endSelection();
    int start = startsel->row * m_cols + startsel->column;
    int end = endsel->row * m_cols + endsel->column;

    for(int i = start; i <= end; i++)
    {
        m_surface[i].background = Theme_SelectionBg;
        m_surface[i].foreground = Theme_SelectionFg;
    }
}

bool Surface::hasFlag(rd_flag flag) const { return m_flags & flag; }
