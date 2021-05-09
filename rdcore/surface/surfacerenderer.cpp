#include "surfacerenderer.h"
#include "../document/document.h"
#include "../context.h"
#include <algorithm>

#define BLANK_CELL { Theme_Default, Theme_Default, ' ' }

SurfaceRenderer::SurfaceRenderer(Context* ctx, rd_flag flags): Object(ctx), m_flags(flags) { }
SafeDocument& SurfaceRenderer::document() const { return this->context()->document(); }
rd_address SurfaceRenderer::firstAddress() const { return m_rows.empty() ? m_range.first : m_rows.front().address; }
rd_address SurfaceRenderer::lastAddress() const { return m_rows.empty() ? m_range.second : m_rows.back().address; }
const SurfaceRenderer::Rows& SurfaceRenderer::rows() const { return m_rows; }
bool SurfaceRenderer::hasFlag(rd_flag flag) const { return m_flags & flag; }

int SurfaceRenderer::row(int row, const RDSurfaceCell** cells) const
{
    Lock lock(m_mutex);
    if(row >= this->lastRow()) return 0;
    int c = m_ncols != -1 ? std::min<int>(m_rows[row].cells.size(), m_ncols) : m_rows[row].cells.size();

    if(cells)
    {
        m_reqrows.resize(c);
        std::copy_n(m_rows[row].cells.begin(), c, m_reqrows.data());
        *cells = m_reqrows.data();
    }

    return c;
}

int SurfaceRenderer::indexOf(rd_address address) const
{
    for(size_t i = 0; i < m_rows.size(); i++)
    {
        if((m_rows[i].address == address) && !m_rows[i].isvirtual)
            return i;
    }

    return -1;
}

int SurfaceRenderer::lastIndexOf(rd_address address) const
{
    for(size_t i = m_rows.size(); i-- > 0; )
    {
        if((m_rows[i].address == address) && !m_rows[i].isvirtual)
            return i;
    }

    return -1;
}

void SurfaceRenderer::setLastColumn(int col) { m_lastcolumn = std::max<int>(this->lastColumn(), col); }

int SurfaceRenderer::getRangeColumn(rd_address address, rd_address endaddress) const
{
    auto it = std::lower_bound(m_rows.begin(), m_rows.end(), address, [](const auto& row, rd_address address) {
        return row.address < address;
    });

    if((it == m_rows.end()) || (it->address != address)) return this->lastColumn();

    int maxcol = 0;

    for( ; (it != m_rows.end()) && (it->address <= endaddress); it++)
        maxcol = std::max<int>(maxcol, it->text.size());

    return maxcol;
}

int SurfaceRenderer::firstColumn() const { return m_firstcol; }
int SurfaceRenderer::lastColumn() const { return m_ncols != -1 ? m_ncols : m_lastcolumn; }
int SurfaceRenderer::lastRow() const { return std::min<int>(m_nrows, m_rows.size()); }

void SurfaceRenderer::updateSegment(const RDSegment& segment, rd_address startaddress, const CanUpdateCallback& canupdate)
{
    const auto* addressspace = this->document()->addressSpace();
    auto* blocks = addressspace->getBlocks(segment.address);
    if(!blocks) return;

    auto it = blocks->find(startaddress);
    if(it == blocks->end()) return;

    if((it->address == segment.address) && !this->hasFlag(RendererFlags_NoSegmentLine))
    {
        size_t segmentidx = addressspace->indexOfSegment(&segment);
        if((segmentidx != RD_NVAL) && segmentidx) this->createEmptyLine(it->address);
        this->createLine(it->address).renderSegment();
    }

    for( ; canupdate(it->address) && (it != blocks->end()); it++)
    {
        rd_flag flags = addressspace->getFlags(it->address);

        switch(it->type)
        {
            case BlockType_Code: {
                if(flags & AddressFlags_Function) {
                    if((it->address != segment.address)) this->createEmptyLine(it->address, true);
                    if(!this->hasFlag(RendererFlags_NoFunctionLine)) this->createLine(it->address).renderFunction();
                }
                else if(flags & AddressFlags_Location) {
                    this->createEmptyLine(it->address);
                    this->createLine(it->address).renderLocation();
                }

                this->createLine(it->address).renderInstruction();
                if(this->document()->isBasicBlockTail(it->address)) this->createSeparator(it->address);
                break;
            }

            case BlockType_String:
            case BlockType_Data: {
                if(flags & AddressFlags_Type) {
                    this->createEmptyLine(it->address);
                    this->createLine(it->address).renderType();
                }

                if(flags & AddressFlags_TypeField) this->createLine(it->address).renderTypeField();
                else if(it->type == BlockType_String) this->createLine(it->address).renderString();
                else this->createLine(it->address).renderData();
                if(flags & AddressFlags_TypeEnd) this->createEmptyLine(it->address);
                break;
            }

            case BlockType_Unknown: this->createLine(it->address).renderUnknown(BlockContainer::size(std::addressof(*it))); break;
            default: this->createLine(it->address).renderLine("Block #" + std::to_string(it->type)); break;
        }
    }
}

void SurfaceRenderer::updateSegments(const CanUpdateCallback& canupdate)
{
    const auto* addressspace = this->document()->addressSpace();

    RDSegment startsegment;
    if(!addressspace->addressToSegment(m_range.first, &startsegment)) return;

    RDSegment segment = startsegment;
    size_t segmentidx = addressspace->indexOfSegment(&startsegment);

    m_needsempty = false;

    for( ; (segmentidx < addressspace->size()) && canupdate(segment.address); segmentidx++)
    {
        if(!addressspace->indexToSegment(segmentidx, &segment)) break;

        rd_address startaddress = (segment == startsegment) ? m_range.first : segment.address;
        this->updateSegment(segment, startaddress, canupdate);
    }

    // Fill remaining cells with blank characters
    for(auto& row : m_rows)
    {
        for(int i = row.cells.size(); i < m_lastcolumn; i++)
            row.cells.push_back(BLANK_CELL);
    }
}

SurfaceRow& SurfaceRenderer::insertRow(rd_address address)
{
    auto& row = m_rows.emplace_back();
    row.address = address;
    return row;
}

void SurfaceRenderer::getSize(int* rows, int* cols) const
{
    if(rows) *rows = m_nrows;
    if(cols) *cols = this->lastColumn();
}

void SurfaceRenderer::update()
{
    if(m_nrows && m_ncols)
        this->resize(m_nrows, m_ncols);
    else if((m_range.first != RD_NVAL) && (m_range.first != m_range.second))
        this->resizeRange(m_range.first, m_range.second, m_ncols);
}

void SurfaceRenderer::update(const CanUpdateCallback& canupdate)
{
    if(m_range.first == RD_NVAL) return;

    m_rows.clear();
    m_lastcolumn = 0;

    this->updateSegments(canupdate);
    this->updateCompleted();
    this->notify<RDEventArgs>(Event_SurfaceUpdated, this);
}

void SurfaceRenderer::resizeRange(rd_address startaddress, rd_address endaddress, int cols)
{
    if((startaddress == RD_NVAL) || (endaddress == RD_NVAL)) return;

    m_range.first = startaddress;
    m_range.second = endaddress;
    m_ncols = cols;

    if(startaddress == 0x000723FC)
    {
        int zzz = 0;
        zzz++;
    }

    this->update([&](rd_address address) {
        return address <= m_range.second;
    });

    m_nrows = m_rows.size();
}

void SurfaceRenderer::resize(int rows, int cols)
{
    int area = rows * cols;
    if(!area) return;

    m_nrows = rows;
    m_ncols = cols;

    this->update([&](rd_address) {
        return this->lastRow() < m_nrows;
    });

    m_range.second = m_rows.empty() ? m_range.first : m_rows[this->lastRow() - 1].address;
}

RDSurfaceCell& SurfaceRenderer::cell(int row, int col) { return m_rows[row].cells.at(col); }
