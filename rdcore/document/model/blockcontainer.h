#pragma once

#include <functional>
#include <algorithm>
#include <vector>
#include <rdapi/document/block.h>
#include <rdapi/types.h>
#include "../../containers/treecontainer.h"

struct BlockSorter
{
    typedef void is_transparent;

    bool operator()(const RDBlock& b1, const RDBlock& b2) const { return b1.end <= b2.start; }
    bool operator()(rd_address address, const RDBlock& b) const { return address < b.start; }
    bool operator()(const RDBlock& b, rd_address address) const { return b.end <= address; }
};

class BlockContainer: public TreeContainer<RDBlock, BlockSorter>
{
    public:
        BlockContainer() = default;
        void explored(rd_address start, rd_address end);
        void unknown(rd_address start, rd_address end);
        void data(rd_address start, rd_address end);
        void code(rd_address start, rd_address end);
        void string(rd_address start, rd_address end);
        void exploredSize(rd_address start, size_t size);
        void unknownSize(rd_address start, size_t size);
        void dataSize(rd_address start, size_t size);
        void codeSize(rd_address start, size_t size, u16 info = 0);
        void stringSize(rd_address start, size_t size);
        void info(rd_address address, rd_type type, u16 info);
        size_t size() const;
        bool empty() const;

    public:
        static size_t size(const RDBlock* b);
        static bool contains(const RDBlock* b, rd_address address);
        static bool empty(const RDBlock* b);

    private:
        ContainerType::const_iterator get(rd_address address) const;
        bool canMerge(const RDBlock* block1, const RDBlock* block2) const;
        bool canMerge(const RDBlock* block, rd_type type) const;
        void mark(rd_address start, rd_address end, rd_type type, u16 info = 0);
        void markSize(rd_address start, size_t size, rd_type type, u16 info = 0);
        void doInsert(const RDBlock& b);

    private:
        template<typename IteratorType> void doRemove(IteratorType startit, IteratorType endit);
        template<typename IteratorType> void doRemove(IteratorType it);
        using TreeContainer<RDBlock, BlockSorter>::insert;
        using TreeContainer<RDBlock, BlockSorter>::remove;

    public:
        using TreeContainer<RDBlock, BlockSorter>::contains;
        using TreeContainer<RDBlock, BlockSorter>::get;
        using TreeContainer<RDBlock, BlockSorter>::size;
};

template<typename IteratorType>
void BlockContainer::doRemove(IteratorType startit, IteratorType endit) { m_container.erase(startit, endit); }

template<typename IteratorType>
void BlockContainer::doRemove(IteratorType it) { m_container.erase(it); }
