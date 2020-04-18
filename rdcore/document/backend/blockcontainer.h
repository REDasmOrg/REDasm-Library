#pragma once

#include <functional>
#include <algorithm>
#include <vector>
#include <rdapi/document/block.h>
#include <rdapi/types.h>
#include "../../eventdispatcher.h"

class BlockContainer
{
    private:
        typedef std::vector<RDBlock> Container;

    public:
        BlockContainer() = default;
        void unexplored(const RDBlock* blockitem);
        void unexplored(address_t start);
        void unexplored(address_t start, address_t end);
        void data(address_t start, address_t end);
        void code(address_t start, address_t end);
        void unexploredSize(address_t start, size_t size);
        void dataSize(address_t start, size_t size);
        void codeSize(address_t start, size_t size);

    public:
        bool find(address_t address, RDBlock* block) const;
        bool get(size_t idx, RDBlock* block) const;
        const RDBlock& at(size_t idx) const;
        size_t indexOf(const RDBlock* b) const;
        size_t size() const;

    public:
        static size_t size(const RDBlock* b);
        static bool contains(const RDBlock* b, address_t address);
        static bool empty(const RDBlock* b);

    private:
        void mark(address_t start, address_t end, type_t type);
        void markSize(address_t start, size_t size, type_t type);
        void insert(address_t start, address_t end, type_t type);
        void remove(address_t start, address_t end);
        template<typename Iterator> Iterator findOverlap(Iterator first, Iterator last, address_t address) const;
        template<typename Iterator> Iterator insertionPoint(Iterator first, Iterator last, address_t address) const;
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
              EventDispatcher::dispatch<RDDocumentBlockEventArgs>(Event_DocumentBlockRemoved, this, b);
              return true;
    });

    return m_blocks.erase(it, endit);
}

template<typename Iterator>
Iterator BlockContainer::eraseBlock(Iterator it)
{
    RDBlock b = *it;
    it = m_blocks.erase(it);
    EventDispatcher::dispatch<RDDocumentBlockEventArgs>(Event_DocumentBlockRemoved, this, b);
    return it;
}

template<typename Iterator>
Iterator BlockContainer::insertBlock(Iterator it, const RDBlock& bi)
{
    auto resit = m_blocks.insert(it, bi);
    EventDispatcher::dispatch<RDDocumentBlockEventArgs>(Event_DocumentBlockInserted, this, *resit);
    return resit;
}

template<typename Iterator>
Iterator BlockContainer::findOverlap(Iterator first, Iterator last, address_t address) const
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
Iterator BlockContainer::insertionPoint(Iterator first, Iterator last, address_t address) const
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
