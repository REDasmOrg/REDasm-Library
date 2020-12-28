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
        typedef std::function<void(const RDBlock&)> Callback;

    public:
        BlockContainer();
        void whenInsert(const Callback& cb);
        void whenRemove(const Callback& cb);
        void unknown(rd_address start, rd_address end);
        void data(rd_address start, rd_address end);
        void code(rd_address start, rd_address end);
        void unknownSize(rd_address start, size_t size);
        void dataSize(rd_address start, size_t size);
        void codeSize(rd_address start, size_t size);

    public:
        static size_t size(const RDBlock* b);
        static bool contains(const RDBlock* b, rd_address address);
        static bool empty(const RDBlock* b);

    private:
        ContainerType::const_iterator get(rd_address address) const;
        void mark(rd_address start, rd_address end, rd_type type);
        void markSize(rd_address start, size_t size, rd_type type);
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

    private:
        Callback m_oninsert, m_onremove;
};

template<typename IteratorType>
void BlockContainer::doRemove(IteratorType startit, IteratorType endit)
{
    std::for_each(startit, endit, [&](const RDBlock& b) { m_onremove(b); });
    m_container.erase(startit, endit);
}

template<typename IteratorType>
void BlockContainer::doRemove(IteratorType it)
{
    m_onremove(*it);
    m_container.erase(it);
}
