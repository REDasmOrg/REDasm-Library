#pragma once

#include <rdapi/support/utils.h>
#include <memory>
#include "../object.h"
#include "../buffer/buffer.h"
#include "../engine/stringfinder.h"
#include "backend/listing.h"

class DocumentData: public Listing
{
    public:
        DocumentData(const MemoryBufferPtr& buffer, Context* ctx);
        rd_endianness endianness() const;
        MemoryBuffer* buffer() const;
        void setEndianness(rd_endianness endianness);

    public:
        size_t getFunctionInstrCount(rd_address address) const;
        bool blockView(rd_address address, RDBufferView* view) const;
        bool view(rd_address address, RDBufferView* view) const;
        bool view(rd_address address, size_t size, RDBufferView* view) const;
        bool view(const RDSegment& segment, RDBufferView* view) const;
        bool view(const RDBlock& block, RDBufferView* view) const;

    public:
        RDLocation dereference(rd_address address) const;
        const char* getHexDump(rd_address address, size_t size) const;
        const char* readString(rd_address address, size_t* len) const;
        const char16_t* readWString(rd_address address, size_t* len) const;
        std::string readString(rd_address address, size_t len = RD_NVAL) const;  // Internal C++ Helper
        std::string readWString(rd_address address, size_t len = RD_NVAL) const; // Internal C++ Helper

    public:
        bool isAddress(rd_address address) const;
        RDLocation offset(rd_address address) const;
        RDLocation address(rd_offset offset) const;
        RDLocation addressof(const void* ptr) const;
        RDLocation fileoffset(const void* ptr) const;
        u8* addrpointer(rd_address address) const;
        u8* offspointer(rd_offset offset) const;

    protected:
        bool readAddress(rd_address address, u64 *value) const;
        bool readAddress(rd_address address, size_t size, u64 *value) const;
        RDLocation dereferenceAddress(rd_address address) const;

    private:
        template<typename T> const T* readStringT(rd_address address, size_t* len) const;

    private:
        rd_endianness m_endianness{Endianness_Little};
        MemoryBufferPtr m_buffer;
};

template<typename T>
const T* DocumentData::readStringT(rd_address address, size_t* len) const {
    size_t clen = 0, maxlen = RD_NVAL;
    if(len && *len) maxlen = *len;

    RDBufferView view;
    if(!this->view(address, &view)) return nullptr;
    const T* pstart = reinterpret_cast<T*>(view.data);

    for(size_t i = 0 ; (i < maxlen) && !BufferView::empty(&view); i++, BufferView::advance(&view, sizeof(T))) {
        if(!StringFinder::toAscii(*reinterpret_cast<T*>(view.data), nullptr)) break;
        clen++;
    }

    if(len) *len = clen;
    return clen ? reinterpret_cast<const T*>(pstart) : nullptr;
}
