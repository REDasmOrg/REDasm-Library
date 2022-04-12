#pragma once

#include <rdapi/document/block.h>
#include <optional>

class AddressSpace;
class BlockContainer;

class BlocksIterator
{
    public:
        BlocksIterator(const AddressSpace* addressspace);
        BlocksIterator(const AddressSpace* addressspace, rd_address address);
        std::optional<RDBlock> current() const;
        std::optional<RDBlock> find(rd_address address);
        std::optional<RDBlock> prev();
        std::optional<RDBlock> next();

    private:
        void seekTo(const RDBlock& b, const BlockContainer* bc);
        bool seekSegment(int dir);

    private:
        const AddressSpace* m_addressspace;
        const BlockContainer* m_blockcontainer{nullptr};
        std::optional<RDBlock> m_currblock;
        size_t m_segmentidx{RD_NVAL};
};
