#pragma once

#include <rdapi/document/document.h>
#include "../../containers/sortedcontainer.h"
#include "blockcontainer.h"

struct SegmentSorter
{
    bool operator()(const RDSegment& segment1, const RDSegment& segment2) const {
        return segment1.address < segment2.address;
    }
};

struct SegmentComparator
{
    bool operator()(const RDSegment& segment1, const RDSegment& segment2) const {
        return std::tie(segment1.name, segment1.flags, segment1.offset, segment1.address, segment1.endoffset, segment1.endaddress) ==
               std::tie(segment2.name, segment2.flags, segment2.offset, segment2.address, segment2.endoffset, segment2.endaddress);
    }
};

class SegmentContainer: public SortedContainer<RDSegment, SegmentComparator, SegmentSorter>
{
    private:
        typedef SortedContainer<RDSegment, SegmentComparator, SegmentSorter> ClassType;

    public:
        SegmentContainer();
        void whenInsert(const BlockContainer::Callback& cb);
        void whenRemove(const BlockContainer::Callback& cb);
        void removeAddress(rd_address address);
        bool markCode(rd_address address, size_t size);
        bool markData(rd_address address, size_t size);
        bool find(rd_address address, RDSegment* segment) const;
        bool findOffset(rd_offset offset, RDSegment* segment) const;
        bool findBlock(rd_address address, RDBlock* block) const;
        bool setUserData(rd_address address, uintptr_t userdata);
        const RDSegment* insert(const RDSegment& segment) override;
        BlockContainer* findBlocks(rd_address address) const;
        BlockContainer* findBlocks(rd_address address);

    public:
        static size_t addressSize(const RDSegment& segment);
        static size_t offsetSize(const RDSegment& segment);
        static bool containsAddress(const RDSegment* segment, rd_address address);
        static bool containsOffset(const RDSegment* segment, rd_offset offset);

    private:
        std::unordered_map<rd_address, BlockContainer> m_blocks;
        BlockContainer::Callback m_oninsert, m_onremove;
};
