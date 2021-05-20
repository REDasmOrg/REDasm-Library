#include "surface.h"
#include "../document/document.h"
#include "../context.h"
#include "renderer.h"
#include <cmath>

Surface::Surface(Context* ctx, rd_flag flags, uintptr_t userdata): SurfaceRenderer(ctx, flags), m_path(this), m_userdata(userdata)
{
    this->unlink();
    this->context()->subscribe(this, std::bind(&Surface::handleEvents, this, std::placeholders::_1));

    RDLocation entry = this->document()->getEntry();
    if(entry.valid) this->goTo(entry.address, false);
    else this->goTo(this->document()->firstAddress(), false);
}

Surface::~Surface()
{
    m_cursor->detach(this);
    this->document()->unsubscribe(this);

    if(this->context()->activeSurface() == this)
        this->context()->setActiveSurface(nullptr);
}

const CursorPtr& Surface::cursor() const { return m_cursor; }
size_t Surface::getPath(const RDPathItem** path) const { return m_path.getPath(path); }
rd_address Surface::currentAddress() const { return m_cursor ? m_cursor->currentAddress() : RD_NVAL; }

const std::string* Surface::currentLabel(rd_address* resaddress) const
{
    auto* cw = this->currentWord();
    if(!cw) return nullptr;

    rd_address address = this->document()->getAddress(*cw);
    if(address == RD_NVAL) return nullptr;

    if(resaddress) *resaddress = address;
    return cw;
}

uintptr_t Surface::userData() const { return m_userdata; }
rd_address Surface::getAddress(int row) const { return (row < this->lastRow()) ? m_rows[row].address : RD_NVAL; }

void Surface::getScrollRange(rd_address* start, rd_address* end) const
{
    if(start) *start = this->document()->firstAddress();
    if(end) *end = this->document()->lastAddress();
}

bool Surface::contains(rd_address address) const
{
    if(m_rows.empty() || (m_nrows >= this->lastRow())) return false;

    return (address >= m_rows.front().address) &&
           (address <= m_rows[m_nrows].address);
}

bool Surface::labelAt(int row, int col, rd_address* resaddress) const
{
    auto* w = this->wordAt(row, col);
    if(!w) return false;

    rd_address address = this->document()->getAddress(w->c_str());
    if(address == RD_NVAL) return false;

    if(resaddress) *resaddress = address;
    return true;
}

const std::string* Surface::currentWord() const { return this->wordAt(m_cursor->currentRow(), m_cursor->currentColumn()); }

const std::string* Surface::wordAt(int row, int col) const
{
    if(row >= this->lastRow()) return nullptr;

    col += m_firstcol; // Fix column according to horizontal scroll
    int currcol = 0;

    for(const auto& ch : m_rows[row].chunks)
    {
        if((col >= currcol) && (col < static_cast<int>(currcol + ch.chunk.size())))
            return std::addressof(ch.chunk);

        currcol += ch.chunk.size();
    }

    return nullptr;
}

const std::string& Surface::selectedText() const { return m_selectedtext; }

bool Surface::goTo(rd_address address, bool updatehistory)
{
    auto* blocks = this->document()->getBlocks(address);
    if(!blocks) return false;

    auto it = blocks->lowerBound(address);
    return (it != blocks->end()) ? this->goTo(std::addressof(*it), updatehistory) : false;
}

void Surface::setUserData(uintptr_t userdata) { m_userdata = userdata; }

void Surface::scroll(rd_address address, int cols)
{
    int oldfirstcol = m_firstcol;
    m_firstcol = std::max(m_firstcol + cols, 0);
    if(address != RD_NVAL) this->scrollAddress(address);

    if((m_range.first != RD_NVAL) || (m_firstcol != oldfirstcol))
    {
        m_cursor->clearSelection();
        this->update();
        this->notifyAddressChanged();
    }
}

void Surface::moveTo(int row, int col)
{
    if(m_rows.empty()) return;
    this->checkColumn(row, col);

    if(row >= m_nrows) this->scroll(m_rows[row - m_nrows].address, m_firstcol);
    else if(row < 0) this->scroll(m_rows.front().address - 1, m_firstcol);
    else
    {
        m_cursor->setCurrentAddress(m_rows[row].address);
        m_cursor->moveTo(row, col);
    }
}

void Surface::select(int row, int col)
{
    if(row < 0) row = 0;
    if(col < 0) col = 0;

    row = std::min(row, this->lastRow() - 1);
    this->checkColumn(row, col);

    m_cursor->select(row, col);
}

void Surface::selectAt(int row, int col)
{
    if(row >= this->lastRow()) return;

    int startcol = 0;

    for(const auto& ch : m_rows[row].chunks)
    {
        int endcol = static_cast<int>(startcol + ch.chunk.size());

        if((col >= startcol) && (col <= endcol))
        {
            this->moveTo(row, startcol);
            this->select(row, endcol - 1);
            break;
        }

        startcol += ch.chunk.size();
    }
}

void Surface::notifyAddressChanged()
{
    rd_address address = this->currentAddress();
    if(address == RD_NVAL) return;

    this->notify<RDSurfaceEventArgs>(Event_SurfaceAddressChanged,
                                     this, m_cursor->position(), m_cursor->selection(), address);

    if(!m_active) return;

    // Notify globally too
    this->context()->notify<RDSurfaceEventArgs>(Event_SurfaceAddressChanged,
                                                this, m_cursor->position(), m_cursor->selection(), address);
}

void Surface::updateCompleted()
{
    if(m_cursor && m_updatecursor)
    {
        for(int i = m_rows.size(); i-- > 0; )
        {
            auto& row = m_rows[i];
            if(row.address != m_cursor->currentAddress()) continue;
            m_cursor->set(i, m_cursor->currentColumn());
            break;
        }

        m_updatecursor = false;
    }

    this->notifyAddressChanged();

    if(!this->hasFlag(RendererFlags_NoCursor)) this->highlightCurrentRow();
    if(!this->hasFlag(RendererFlags_NoHighlightWords)) this->highlightWords();

    if(!this->hasFlag(RendererFlags_NoCursor))
    {
        this->checkSelection();
        this->drawCursor();
    }

    if(!this->context()->busy()) m_path.update();
}

void Surface::checkColumn(int row, int& col) const
{
    int lastcol = this->lastColumn();
    if(col == -1) col = lastcol - 1;
    else col = std::min(col, lastcol);

    if((row >= 0) && (row < this->lastRow()))
        col = std::min<int>(col, m_rows[row].cells.size() - 1);
}

void Surface::scrollAddress(rd_address address)
{
    const AddressSpace* addressspace = this->document()->addressSpace();

    if(address < addressspace->firstAddress())
        address = addressspace->firstAddress();
    else if(address > addressspace->lastAddress())
        address = addressspace->lastAddress();
    else
    {
        auto* blocks = addressspace->getBlocks(address);
        if(!blocks) return;

        auto it = blocks->end();

        if(address < m_range.first) // Backward scroll
        {
            it = blocks->lowerBound(address);

            if(it == blocks->end()) // Intra-segment rendering
            {
                size_t idx = addressspace->indexOfSegment(address);
                if(!idx) return;
                blocks = addressspace->getBlocksAt(idx - 1);
                if(!blocks || blocks->empty()) return;
                it = --blocks->end();
            }
        }
        else // Forward scroll
        {
            it = blocks->upperBound(address);

            if(it == blocks->end()) // Intra-segment rendering
            {
                size_t idx = addressspace->indexOfSegment(address);
                if(idx >= addressspace->size()) return;
                blocks = addressspace->getBlocksAt(idx + 1);
                if(!blocks || blocks->empty()) return;
                it = blocks->begin();
            }
        }

        if(it == blocks->end()) return;
        address = it->address;
    }

    m_range.first = address;
}

void Surface::handleEvents(const RDEventArgs* event)
{
    switch(event->id)
    {
        case Event_BusyChanged: {
            if(this->context()->busy()) return;
            RDLocation loc = this->document()->getEntry();
            if(loc.valid) this->goTo(loc.address, false);
            break;
        }

        case Event_ContextFlagsChanged: break;
        default: return;
    }

    this->update();
}

void Surface::notifyHistoryChanged()
{
    this->notify<RDEventArgs>(Event_SurfaceHistoryChanged, this);
    if(!m_active) return;
    this->context()->notify<RDEventArgs>(Event_SurfaceHistoryChanged, this); // Notify globally too
}

void Surface::linkTo(Surface* s)
{
    if(!s) return;

    m_cursor->linkHistory(s->cursor()->history());

    // if(m_cursor) m_cursor->detach(this);
    // m_cursor = s->cursor();
    // m_cursor->attach(this);

    // if(s->m_items.first.type)
    //     this->goTo(&s->m_items.first);
}

void Surface::unlink()
{
    if(m_cursor)
    {
        //m_cursor->detach(this);
        m_cursor->unlinkHistory();
        return;
    }

    m_cursor = std::make_shared<Cursor>(this->context());
    m_cursor->attach(this);
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

bool Surface::goTo(const RDBlock* block, bool updatehistory)
{
    if(!block) return false;

    auto* blocks = this->document()->getBlocks(block->address);
    if(!blocks) return false;
    if(updatehistory && m_cursor) m_cursor->updateHistory();

    if(this->hasFlag(RendererFlags_CenterOnCursor))
    {
        if(!this->ensureVisible(blocks, block->address))
            return false;
    }
    else m_range.first = block->address;

    if(m_cursor)
    {
        m_updatecursor = true;
        m_cursor->setCurrentAddress(block->address);
    }

    this->update();
    return true;
}

void Surface::drawCursor()
{
    if(!this->active()) return;
    if(m_cursor->currentColumn() >= this->lastColumn()) return;
    if(m_cursor->currentRow() >= this->lastRow()) return;

    auto& cell = this->cell(m_cursor->currentRow(), m_cursor->currentColumn());
    cell.background = Theme_CursorBg;
    cell.foreground = Theme_CursorFg;
}

void Surface::highlightCurrentRow()
{
    if(!this->active() || (m_cursor->currentRow() >= this->lastRow())) return;

    for(RDSurfaceCell& cell : m_rows[m_cursor->currentRow()].cells)
        cell.background = Theme_Seek;
}

void Surface::highlightWords()
{
    if(!this->active() || m_cursor->hasSelection()) return;

    auto* cw = this->currentWord();
    if(!cw || (cw->find_first_not_of(' ') == std::string::npos)) return;

    int lastcol = this->lastColumn();

    for(size_t row = 0; row < m_rows.size(); row++)
    {
        const auto& sfrow = m_rows[row];
        int col = -m_firstcol; // Fix column according to horizontal scroll

        for(const auto& c : sfrow.chunks)
        {
            for(int i = 0; (*cw == c.chunk) && (i < static_cast<int>(c.chunk.size())); i++)
            {
                int currcol = col + i;
                if(currcol < 0) continue;
                if(currcol >= lastcol) break;

                this->cell(row, currcol).background = Theme_HighlightBg;
                this->cell(row, currcol).foreground = Theme_HighlightFg;
            }

            col += c.chunk.size();
        }
    }
}

void Surface::checkSelection()
{
    m_selectedtext.clear();
    if(!m_cursor->hasSelection()) return;

    const auto* startsel = m_cursor->startSelection();
    const auto* endsel = m_cursor->endSelection();

    for(int row = startsel->row; row <= endsel->row; row++)
    {
        int startcol = 0, endcol = m_rows[row].cells.size() - 1;
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

bool Surface::ensureVisible(const BlockContainer* blocks, rd_address address)
{
    auto it = blocks->find(address);
    if(it == blocks->end()) return false;

    // Center on view
    for(int i = 0; i < (m_nrows / 4) && (it != blocks->begin()); i++) it--;
    m_range.first = it->address;
    return true;
}
