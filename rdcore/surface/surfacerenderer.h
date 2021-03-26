#pragma once

#include <unordered_map>
#include <vector>
#include <deque>
#include <mutex>
#include <rdapi/renderer/surface.h>
#include "../document/document_fwd.h"
#include "../object.h"
#include "renderer.h"
#include "common.h"

struct SurfaceColumns
{
    size_t instrstartcol{0};
    size_t mnemonicendcol{0};
};

class SurfaceRenderer: public Object
{
    public:
        typedef std::deque<SurfaceRow> Rows;

    private:
        typedef std::scoped_lock<std::mutex> Lock;

        struct LastState {
            rd_type blocktype{BlockType_Unknown};
            rd_flag flags{AddressFlags_None};
        };

    public:
        SurfaceRenderer(Context* ctx, rd_flag flags);
        SafeDocument& document() const;
        const Rows& rows() const;
        bool hasFlag(rd_flag flag) const;
        int indexOf(rd_address address) const;
        int lastIndexOf(rd_address address) const;
        int firstColumn() const;
        int lastColumn() const;
        int lastRow() const;
        int row(int row, const RDSurfaceCell** cells) const;
        void setLastColumn(int col);
        void update(rd_address currentaddress = RD_NVAL);

    protected:
        RDSurfaceCell& cell(int row, int col);
        virtual void updateCompleted(rd_address currentaddress) = 0;
        void updateSegment(const RDSegment* segment, size_t segmentidx, rd_address startaddress);
        void updateSegments();

    private:
        inline Renderer createLine(rd_address address);
        inline void createEmptyLine(rd_address address);
        SurfaceRow& insertRow(rd_address address);
        bool needsRows() const;

    protected:
        mutable std::mutex m_mutex;
        std::unordered_map<rd_address, LastState> m_laststate;
        std::pair<rd_address, rd_address> m_range{RD_NVAL, RD_NVAL};
        int m_nrows{0}, m_ncols{0}, m_firstcol{0}, m_lastcolumn{0}, m_lastempty{false};
        rd_flag m_flags;
        Rows m_rows;

    private:
        mutable std::vector<RDSurfaceCell> m_reqrows;
};

inline Renderer SurfaceRenderer::createLine(rd_address address) {
    m_lastempty = false;
    return Renderer(this, this->insertRow(address), m_flags);
}

inline void SurfaceRenderer::createEmptyLine(rd_address address) {
    if(this->hasFlag(RendererFlags_NoEmptyLine) || m_lastempty) return;
    this->createLine(address);
    m_lastempty = true;
}

