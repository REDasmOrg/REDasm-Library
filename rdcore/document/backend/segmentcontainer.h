#pragma once

#include <unordered_map>
#include <rdapi/document/document.h>
#include "../../containers/treecontainer.h"
#include "blockcontainer.h"

struct SegmentSorter
{
    typedef void is_transparent;

    bool operator()(const RDSegment& segment1, const RDSegment& segment2) const {
        return segment1.address < segment2.address;
    }

    bool operator()(rd_address address, const RDSegment& s) const { return address < s.address; }
    bool operator()(const RDSegment& s, rd_address address) const { return s.address < address; }
};

class SegmentContainer: public TreeContainer<RDSegment, SegmentSorter>
{
    private:
        typedef TreeContainer<RDSegment, SegmentSorter> ClassType;

    public:
        SegmentContainer();
        void whenInsert(const BlockContainer::Callback& cb);
        void whenRemove(const BlockContainer::Callback& cb);
        void removeAddress(rd_address address);
        bool markUnknown(rd_address address, size_t size);
        bool markExplored(rd_address address, size_t size);
        bool markCode(rd_address address, size_t size);
        bool markData(rd_address address, size_t size);
        bool find(rd_address address, RDSegment* segment) const;
        bool findOffset(rd_offset offset, RDSegment* segment) const;
        bool findBlock(rd_address address, RDBlock* block) const;
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
