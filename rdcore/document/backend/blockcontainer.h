#pragma once

#include <functional>
#include <algorithm>
#include <vector>
#include <rdapi/document/block.h>
#include <rdapi/types.h>
#include "../../object.h"

class BlockContainer: public Object
{
    private:
        typedef std::vector<RDBlock> Container;
        typedef std::function<void(const RDBlock&)> Callback;

    public:
        Callback blockInserted;
        Callback blockRemoved;

    public:
        BlockContainer() = default;
        void unexplored(const RDBlock* blockitem);
        void unexplored(rd_address start);
        void unexplored(rd_address start, rd_address end);
        void data(rd_address start, rd_address end);
        void code(rd_address start, rd_address end);
        void unexploredSize(rd_address start, size_t size);
        void dataSize(rd_address start, size_t size);
        void codeSize(rd_address start, size_t size);

    public:
        bool find(rd_address address, RDBlock* block) const;
        bool get(size_t idx, RDBlock* block) const;
        const RDBlock& at(size_t idx) const;
        size_t indexOf(const RDBlock* b) const;
        size_t size() const;

    public:
        static size_t size(const RDBlock* b);
        static bool contains(const RDBlock* b, rd_address address);
        static bool empty(const RDBlock* b);

    private:
        void mark(rd_address start, rd_address end, rd_type type);
        void markSize(rd_address start, size_t size, rd_type type);
        void insert(rd_address start, rd_address end, rd_type type);
        void remove(rd_address start, rd_address end);
        template<typename Iterator> Iterator findOverlap(Iterator first, Iterator last, rd_address address) const;
        template<typename Iterator> Iterator insertionPoint(Iterator first, Iterator last, rd_address address) const;
        template<typename Iterator> Iterator eraseRange(Iterator startit, Iterator endit);
        template<typename Iterator> Iterator eraseBlock(Iterator it);
        template<typename Iterator> Iterator insertBlock(Iterator it, const RDBlock& bi);

    private:
        Container m_blocks;
};

template<typename Iterator>
Iterator BlockContainer::eraseRange(Iterator startit, Iterator endit)
{
    auto it = std::remove_if(startit, endit, [&](RDBlock& b) -> bool {
              if(blockRemoved) blockRemoved(b);
              return true;
    });

    return m_blocks.erase(it, endit);
}

template<typename Iterator>
Iterator BlockContainer::eraseBlock(Iterator it)
{
    RDBlock b = *it;
    it = m_blocks.erase(it);
    if(blockRemoved) blockRemoved(b);
    return it;
}

template<typename Iterator>
Iterator BlockContainer::insertBlock(Iterator it, const RDBlock& bi)
{
    auto resit = it;

    if((it != m_blocks.end()) && ((it->type == bi.type) && IS_TYPE(&bi, BlockType_Unexplored)))
    {
        if((it->start - bi.end) == 1)
        {
            if(blockRemoved) blockRemoved(*it);
            it->start = bi.start;
        }
        else if((bi.start - it->end) == 1)
        {
            if(blockRemoved) blockRemoved(*it);
            it->end = bi.end;
        }
        else resit = m_blocks.insert(it, bi);
    }
    else
        resit = m_blocks.insert(it, bi);

    if(blockInserted) blockInserted(*resit);
    return resit;
}

template<typename Iterator>
Iterator BlockContainer::findOverlap(Iterator first, Iterator last, rd_address address) const
{
    typename Iterator::difference_type count = std::distance(first, last), step;
    Iterator it;

    while(count > 0)
    {
        it = first;
        step = count / 2;
        std::advance(it, step);

        if(BlockContainer::contains(std::addressof(*it), address))
            return it;

        if(it->start < address)
        {
            first = ++it;
            count -= step + 1;
        }
        else
            count = step;
    }

    return last;
}

template<typename Iterator>
Iterator BlockContainer::insertionPoint(Iterator first, Iterator last, rd_address address) const
{
    auto count = std::distance(first, last);

    while(count > 0)
    {
        Iterator it = first;
        auto step = count / 2;
        std::advance(it, step);

        if(it->start < address)
        {
            first = ++it;
            count -= step + 1;
        }
        else
            count = step;
    }

    return first;
}

namespace std {

template<>
struct hash<RDBlock> {
    size_t operator()(const RDBlock& b) const {
        return b.start ^ b.end;
    }
};

template<>
struct equal_to<RDBlock> {
    size_t operator()(const RDBlock& b1, const RDBlock& b2) const {
        return (b1.start == b2.start) &&
               (b1.end == b2.end) &&
               (b1.type == b2.type);
    }
};

} // namespace std
