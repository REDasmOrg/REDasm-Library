#pragma once

#include <functional>
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
        typedef std::function<bool(rd_address)> CanUpdateCallback;
        typedef std::scoped_lock<std::mutex> Lock;

        struct LastState {
            rd_type blocktype{BlockType_Unknown};
            rd_flag flags{AddressFlags_None};
        };

    public:
        SurfaceRenderer(Context* ctx, rd_flag flags);
        SafeDocument& document() const;
        rd_address firstAddress() const;
        rd_address lastAddress() const;
        const Rows& rows() const;
        bool hasFlag(rd_flag flag) const;
        int indexOf(rd_address address) const;
        int lastIndexOf(rd_address address) const;
        int firstColumn() const;
        int lastColumn() const;
        int lastRow() const;
        int row(int row, const RDSurfaceCell** cells) const;
        void setLastColumn(int col);
        int getRangeColumn(rd_address address, int rows) const;
        void getSize(int* rows, int* cols) const;
        void update();
        void resizeRange(rd_address startaddress, rd_address endaddress, int cols);
        void resize(int rows, int cols);

    protected:
        RDSurfaceCell& cell(int row, int col);
        void updateSegment(const RDSegment& segment, rd_address startaddress, const CanUpdateCallback& canupdate);
        void updateSegments(const CanUpdateCallback& canupdate);
        virtual void updateCompleted() = 0;

    private:
        inline Renderer createLine(rd_address address, bool isvirtual = false);
        inline void createEmptyLine(rd_address address, bool ignorestate = false);
        inline void createSeparator(rd_address address);
        void update(const CanUpdateCallback& canupdate);
        SurfaceRow& insertRow(rd_address address);

    protected:
        std::pair<rd_address, rd_address> m_range{RD_NVAL, RD_NVAL};
        int m_nrows{0}, m_ncols{0}, m_firstcol{0};
        Rows m_rows;

    private:
        mutable std::mutex m_mutex;
        mutable std::vector<RDSurfaceCell> m_reqrows;
        int m_lastcolumn{0}, m_needsempty{false};
        rd_flag m_flags;
};

inline Renderer SurfaceRenderer::createLine(rd_address address, bool isvirtual) {
    m_needsempty = true;
    auto& row = this->insertRow(address);
    row.isvirtual = isvirtual;
    return Renderer(this, row, m_flags);
}

inline void SurfaceRenderer::createEmptyLine(rd_address address, bool ignorestate) {
    if(this->hasFlag(RendererFlags_NoEmptyLine) || (!ignorestate && !m_needsempty)) return;
    this->createLine(address, true);
    m_needsempty = false;
}

inline void SurfaceRenderer::createSeparator(rd_address address) {
    if(this->hasFlag(RendererFlags_NoSeparatorsLine)) return;
    this->createLine(address, true).renderSeparator();
    m_needsempty = false;
}

