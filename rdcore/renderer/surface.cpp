#include "surface.h"
#include "../document/document.h"
#include "../context.h"
#include "renderer.h"
#include <cmath>

#define BLANK_CELL { Theme_Default, Theme_Default, ' ' }

Surface::Surface(Context* ctx, rd_flag flags, uintptr_t userdata): Cursor(ctx), m_path(this), m_userdata(userdata), m_flags(flags)
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

Surface::~Surface()
{
    this->document()->unsubscribe(this);

    if(this->context()->activeSurface() == this)
        this->context()->setActiveSurface(nullptr);
}

size_t Surface::getPath(const RDPathItem** path) const { return m_path.getPath(path); }

int Surface::row(int row, const RDSurfaceCell** cells) const
{
    SurfaceLock lock(m_mutex);
    auto it = m_surface.find(row);
    if(it == m_surface.end()) return 0;

    int c = m_cols != -1 ? std::min<int>(it->second.cells.size(), m_cols) :
                           it->second.cells.size();

    if(cells)
    {
        std::copy_n(it->second.cells.begin(), c, m_reqrows.data());
        *cells = m_reqrows.data();
    }

    return c;
}

bool Surface::currentItem(RDDocumentItem* item) const
{
    auto it = m_surface.find(this->currentRow());
    if(it == m_surface.end()) return false;

    if(item) *item = it->second.item;
    return true;
}

const RDDocumentItem* Surface::firstItem() const { return &m_items.first; }
const RDDocumentItem* Surface::lastItem() const { return &m_items.second; }
uintptr_t Surface::userData() const { return m_userdata; }

int Surface::findRow(const RDDocumentItem* item) const
{
    for(const auto& ri : m_surface)
    {
        if(!ItemContainer::equals(&ri.second.item, item)) continue;
        return ri.first;
    }

    return -1;
}

bool Surface::getItem(int row, RDDocumentItem* item) const
{
    auto it = m_surface.find(row);
    if(it == m_surface.end()) return false;

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
    auto it = m_surface.find(row);
    if(it == m_surface.end()) return nullptr;

    int currcol = 0;

    for(const std::string& c : it->second.chunks)
    {
        if((col >= currcol) && (col < static_cast<int>(currcol + c.size())))
            return std::addressof(c);

        currcol += c.size();
    }

    return nullptr;
}

const std::string& Surface::selectedText() const { return m_selectedtext; }

bool Surface::goTo(const RDDocumentItem* item)
{
    if(!item) return false;

    auto* items = this->items();
    auto it = items->find(*item);
    if(it == items->end()) return false;

    // Center on view
    for(int i = 0; i < (m_rows / 2) && (it != items->begin()); i++) it--;

    m_items.first = *it;
    this->update(item);
    return true;
}

bool Surface::goToAddress(rd_address address)
{
    auto* items = this->items();
    auto it = items->find(address);
    if(it == items->end()) return false;

    auto currit = it;

    // Select something inside segments
    while((currit != items->end()) && (currit->type <= DocumentItemType_Segment))
    {
        if(currit->address != address) break;
        currit++;
    }

    return this->goTo(std::addressof(*currit));
}

void Surface::getSize(int* rows, int* cols) const
{
    if(rows) *rows = m_rows;
    if(cols) *cols = this->lastColumn();
}

void Surface::scroll(int nrows, int ncols)
{
    this->scrollRows(nrows);
    this->update();

    if(nrows) this->notifyPositionChanged();
}

void Surface::resize(int rows, int cols)
{
    int area = rows * cols;
    if(!area) return;

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

void Surface::selectAt(int row, int col)
{
    auto it = m_surface.find(row);
    if(it == m_surface.end()) return;

    int startcol = 0;

    for(const std::string& chunk : it->second.chunks)
    {
        int endcol = static_cast<int>(startcol + chunk.size());

        if((col >= startcol) && (col <= endcol))
        {
            this->moveTo(row, startcol);
            this->select(row, endcol - 1);
            break;
        }

        startcol += chunk.size();
    }
}

void Surface::onPositionChanged()
{
    this->update();
    this->notifyPositionChanged();
}

const Surface::SurfaceRow* Surface::currentSurfaceRow() const
{
    auto it = m_surface.find(this->currentRow());
    return (it != m_surface.end()) ? std::addressof(it->second) : nullptr;
}

RDSurfaceCell& Surface::cell(size_t row, size_t col) { return m_surface.at(row).cells.at(col); }
SafeDocument& Surface::document() const { return this->context()->document(); }
const ItemContainer* Surface::items() const { return this->context()->document()->items(); }

int Surface::lastColumn() const
{
    if(m_cols == -1) return m_lastcolumn;
    return m_cols;
}

void Surface::notifyPositionChanged()
{
    RDDocumentItem item;
    if(!this->currentItem(&item)) return;
    this->notify<RDSurfaceEventArgs>(Event_SurfacePositionChanged,
                                     this, this->position(), this->selection(), item);

    if(!m_active) return;

    // Notify globally too
    this->context()->notify<RDSurfaceEventArgs>(Event_SurfacePositionChanged,
                                                this, this->position(), this->selection(), item);
}

void Surface::checkColumn(int row, int& col) const
{
    int lastcol = this->lastColumn();
    if(col == -1) col = lastcol - 1;
    else col = std::min(col, lastcol);

    auto it = m_surface.find(row);
    if(it != m_surface.end()) col = std::min<int>(col, it->second.cells.size() - 1);
}

void Surface::scrollRows(int nrows)
{
    if(!nrows) return;
    this->clearSelection();

    auto* items = this->items();
    auto it = items->find(m_items.first);
    if(it == items->end()) return;

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
    switch(event->id)
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

void Surface::onStackChanged() { this->notify<RDEventArgs>(Event_SurfaceStackChanged, this); }

void Surface::update(const RDDocumentItem* currentitem)
{
    if(!m_items.first.type || !m_rows || !m_cols) return;

    auto lock = s_lock_safe_ptr(this->document());
    const auto* items = lock->items();
    auto it = items->find(m_items.first);

    // Clear Cached Data
    m_surface.clear();
    m_lastcolumn = 0;

    for(int row = 0; row < m_rows; row++)
    {
        if(it == items->end()) // Fill remaining rows
        {
            m_surface[row].item = { };
            m_surface[row].cells.assign(m_lastcolumn, BLANK_CELL);
            continue;
        }

        Renderer r(m_context, m_flags, &m_commentcolumn);
        if(!r.render(std::addressof(*it))) continue;

        m_surface[row].item = *it;
        this->drawRow(m_surface[row], r);

        m_lastcolumn = std::max<int>(m_lastcolumn, m_surface[row].cells.size());
        m_items.second = *it;
        it++;
    }

    // Uniform cells width & highlight item (if any)
    for(auto& [row, surfacerow] : m_surface)
    {
        surfacerow.cells.resize(m_lastcolumn, BLANK_CELL);

        if(ItemContainer::equals(currentitem, &surfacerow.item))
            this->set(row, this->currentColumn());
    }

    m_reqrows.resize(m_lastcolumn); // Prepare buffer for Surface::row()

    if(!this->hasFlag(RendererFlags_NoCursor)) this->highlightCurrentRow();
    if(!this->hasFlag(RendererFlags_NoHighlightWords)) this->highlightWords();

    if(!this->hasFlag(RendererFlags_NoCursor))
    {
        this->checkSelection();
        this->drawCursor();
    }

    if(!m_context->busy()) m_path.update();
    this->notify<RDEventArgs>(Event_SurfaceUpdated, this);
}

void Surface::activate()
{
    if(m_active) return;
    m_active = true;

    this->update();
    this->context()->setActiveSurface(this);
}

void Surface::deactivate()
{
    if(!m_active) return;
    m_active = false;
    this->update();
}

bool Surface::active() const { return m_active; }

void Surface::drawRow(SurfaceRow& sfrow, const Renderer& st)
{
    for(const auto& c : st.chunks())
    {
        sfrow.chunks.push_back(c.chunk);

        for(const auto& ch : c.chunk)
            sfrow.cells.push_back({ c.background, c.foreground, ch });
    }

    // Fill remaining cells with blank characters
    for(int i = sfrow.cells.size() - 1; i < m_cols; i++)
        sfrow.cells.push_back(BLANK_CELL);
}

void Surface::drawCursor()
{
    if(!this->active()) return;
    if(this->currentRow() >= m_rows) return;
    if(this->currentColumn() >= this->lastColumn()) return;

    auto& cell = this->cell(this->currentRow(), this->currentColumn());
    cell.background = Theme_CursorBg;
    cell.foreground = Theme_CursorFg;
}

void Surface::highlightCurrentRow()
{
    if(!this->active() || (this->currentRow() >= m_rows)) return;

    auto it = m_surface.find(this->currentRow());
    if(it == m_surface.end()) return;

    for(RDSurfaceCell& cell : it->second.cells)
        cell.background = Theme_Seek;
}

void Surface::highlightWords()
{
    if(!this->active() || this->hasSelection()) return;

    auto* cw = this->currentWord();
    if(!cw || (cw->find_first_not_of(' ') == std::string::npos)) return;

    int lastcol = this->lastColumn();

    for(const auto& [row, surfacerow] : m_surface)
    {
        int col = 0;

        for(const auto& c : surfacerow.chunks)
        {
            for(int i = 0; (*cw == c) && (i < static_cast<int>(c.size())); i++)
            {
                if((col + i) >= lastcol) break;
                this->cell(row, col + i).background = Theme_HighlightBg;
                this->cell(row, col + i).foreground = Theme_HighlightFg;
            }

            col += c.size();
        }
    }
}

void Surface::checkSelection()
{
    m_selectedtext.clear();
    if(!this->hasSelection()) return;

    const RDSurfacePos* startsel = this->startSelection();
    const RDSurfacePos* endsel = this->endSelection();

    for(int row = startsel->row; row <= endsel->row; row++)
    {
        int startcol = 0, endcol = m_surface[row].cells.size() - 1;
        if(row == startsel->row) startcol = startsel->col;
        if(row == endsel->row) endcol = endsel->col;

        for(int col = startcol; col <= endcol; col++)
        {
            auto& cell = this->cell(row, col);
            cell.background = Theme_SelectionBg;
            cell.foreground = Theme_SelectionFg;
            m_selectedtext += cell.ch;
        }
    }
}

bool Surface::hasFlag(rd_flag flag) const { return m_flags & flag; }
