#include "surface.h"
#include "../document/document.h"
#include "../context.h"
#include "renderer.h"
#include <cmath>

Surface::Surface(Context* ctx, rd_flag flags): Cursor(ctx), m_flags(flags)
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

int Surface::row(int row, RDSurfaceCell* cells) const
{
    auto it = m_visiblerows.find(row);
    if(it == m_visiblerows.end()) return 0;

    SurfaceLock lock(m_mutex);
    if(cells) std::copy_n(std::addressof(m_surface[row * m_cols]), m_cols, cells);
    return m_cols;
}

bool Surface::getCurrentItem(RDDocumentItem* item) const
{
    auto it = m_visiblerows.find(this->currentRow());
    if(it == m_visiblerows.end()) return false;

    if(item) *item = it->second.item;
    return true;
}

bool Surface::getSelectedSymbol(RDSymbol* symbol) const
{
    auto* cw = this->currentWord();
    if(!cw) return false;
    return this->document()->symbol(cw->c_str(), symbol);
}

const std::string* Surface::currentWord() const
{
    auto* surfacerow = this->currentSurfaceRow();
    if(!surfacerow) return nullptr;

    int col = 0;

    for(const std::string& c : surfacerow->chunks)
    {
        if((this->currentColumn() >= col) && (this->currentColumn() < static_cast<int>(col + c.size())))
            return std::addressof(c);

        col += c.size();
    }

    return nullptr;
}

bool Surface::goTo(const RDDocumentItem* item)
{
    if(!item) return false;

    auto it = this->items()->find(*item);
    if(it == this->items()->end()) return false;

    m_items.first = *it;
    this->draw();
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
    this->draw();

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
    this->draw();
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
    this->draw();
    this->notifyChanged();
}

const Surface::SurfaceRow* Surface::currentSurfaceRow() const
{
    auto it = m_visiblerows.find(this->currentRow());
    return (it != m_visiblerows.end()) ? std::addressof(it->second) : nullptr;
}

RDSurfaceCell* Surface::cell(size_t row, size_t col) { return std::addressof(m_surface[row * m_cols + col]); }
SafeDocument& Surface::document() const { return this->context()->document(); }
const ItemContainer* Surface::items() const { return this->context()->document()->items(); }

void Surface::notifyChanged()
{
    RDDocumentItem item;
    if(!this->getCurrentItem(&item)) return;
    this->notify<RDSurfaceEventArgs>(Event_SurfaceChanged, this, this->position(), this->selection(), item);
}

void Surface::checkColumn(int row, int& column) const
{
    if(column == -1) column = m_cols - 1;
    else column = std::min(column, m_cols);

    auto it = m_visiblerows.find(row);
    if(it != m_visiblerows.end()) column = std::min(column, it->second.length - 1);
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

bool Surface::inRange(const RDDocumentItem* item) const
{
    if(!m_items.first.type) return false;
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
                    if(!this->inRange(&e->item)) return;
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

    this->draw();
}

void Surface::draw()
{
    if(!m_items.first.type || !m_rows || !m_cols) return;

    auto lock = s_lock_safe_ptr(this->document());
    const auto* items = lock->items();
    auto it = items->find(m_items.first);
    int row = 0;

    // Clear Cached Data
    std::fill(m_surface.begin(), m_surface.end(), RDSurfaceCell{ Theme_Default, Theme_Default, ' ' });
    m_visiblerows.clear();

    for( ; (row < m_rows) && (it != items->end()); row++, it++)
    {
        Renderer r(m_context, m_flags, &m_commentcolumn);
        if(!r.render(std::addressof(*it))) continue;

        m_visiblerows[row].item = *it;
        this->drawRow(row, r, m_visiblerows[row]);

        m_items.second = *it;
    }

    if(!this->hasFlag(SurfaceFlags_NoHighlightWords)) this->highlightWords();
    if(!this->hasFlag(SurfaceFlags_NoCursor)) this->drawCursor();
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

            u8 bg = c.background, fg = c.foreground;
            this->checkSelection(row, col, &bg, &fg);
            *this->cell(row, col) = { bg, fg, ch };
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

void Surface::highlightWords()
{
    auto* cw = this->currentWord();
    if(!cw) return;

    for(const auto& [row, surfacerow] : m_visiblerows)
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

void Surface::checkSelection(int row, int col, u8* bg, u8* fg)
{
    if(!this->hasSelection()) return;
    const RDCursorPos* startsel = this->startSelection();
    const RDCursorPos* endsel = this->endSelection();

    if((row < startsel->row) || (row > endsel->row)) return;
    if((row == startsel->row) && (col < startsel->column)) return;
    if((row == endsel->row) && (col > endsel->column)) return;

    *bg = Theme_SelectionBg;
       *fg = Theme_SelectionFg;
}

bool Surface::hasFlag(rd_flag flag) const { return m_flags & flag; }
