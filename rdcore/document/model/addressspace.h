#pragma once

#include <unordered_map>
#include "../../containers/addresscontainer.h"
#include "../../buffer/buffer.h"
#include "../../object.h"
#include "blockcontainer.h"

bool operator ==(const RDSegment& s1, const RDSegment& s2);

class AddressSpace: public Object
{
    public:
        AddressSpace(Context* ctx);
        bool markUnknown(rd_address address, size_t size);
        bool markExplored(rd_address address, size_t size);
        bool markCode(rd_address address, size_t size);
        bool markData(rd_address address, size_t size);
        bool markString(rd_address address, size_t size);
        inline BlockContainer* findBlocks(rd_address address) const { return const_cast<AddressSpace*>(this)->findBlocks(address); }
        BlockContainer* findBlocks(rd_address address);

    public:
        const BlockContainer* getBlocks(rd_address address) const;
        const BlockContainer* getBlocksAt(size_t index) const;
        rd_address firstAddress() const;
        rd_address lastAddress() const;
        size_t size() const;
        size_t data(const rd_address** addresses) const;
        size_t indexOfSegment(const RDSegment* segment) const;
        size_t indexOfSegment(rd_address address) const;
        bool pointerToSegment(const void* ptr, RDSegment* segment) const;
        bool addressToSegment(rd_address address, RDSegment* segment) const;
        bool offsetToSegment(rd_offset offset, RDSegment* segment) const;
        bool addressToBlock(rd_address address, RDBlock* block) const;
        bool indexToSegment(size_t index, RDSegment* segment) const;
        bool addressToView(rd_address address, size_t size, RDBufferView* view) const;
        bool offsetToView(rd_offset offset, size_t size, RDBufferView* view) const;
        bool insert(const RDSegment& segment);
        RDLocation offset(rd_address address) const;
        RDLocation address(rd_offset offset) const;
        RDLocation addressof(const void* ptr) const;
        u8* addrpointer(rd_address address) const;
        u8* offspointer(rd_offset offset) const;

    public: // Serialization
        const MemoryBuffer* getBuffer(rd_address address) const;

    public:
        static size_t addressSize(const RDSegment& segment);
        static size_t offsetSize(const RDSegment& segment);
        static bool containsAddress(const RDSegment* segment, rd_address address);
        static bool containsOffset(const RDSegment* segment, rd_offset offset);

    private:
        AddressContainer<RDSegment> m_segments;
        std::unordered_map<rd_address, MemoryBuffer> m_buffers;
        std::unordered_map<rd_address, BlockContainer> m_blocks;
};
