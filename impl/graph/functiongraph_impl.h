#pragma once

#include <redasm/graph/functiongraph.h>
#include <redasm/pimpl.h>
#include <unordered_set>
#include <queue>
#include "graph_impl.h"

namespace REDasm {

class FunctionBasicBlockImpl
{
    PIMPL_DECLARE_Q(FunctionBasicBlock)
    PIMPL_DECLARE_PUBLIC(FunctionBasicBlock)

    public:
        FunctionBasicBlockImpl();
        FunctionBasicBlockImpl(Node n, ListingItem* startitem);

    private:
        Node m_node;
        ListingItem *m_startitem, *m_enditem;                       // [startitem, enditem]
        ListingItem *m_startinstructionitem, *m_endinstructionitem; // [startinstructionitem, endinstructionitem]
        std::unordered_map<Node, String> m_styles;
};

class FunctionGraphImpl: public GraphImpl
{
    PIMPL_DECLARE_Q(FunctionGraph)
    PIMPL_DECLARE_PUBLIC(FunctionGraph)

    private:
        typedef std::unordered_set<ListingItem*> VisitedItems;
        typedef std::queue<ListingItem*> WorkList;

    public:
        FunctionGraphImpl();
        size_t bytesCount() const;
        bool contains(address_t address) const;
        bool build(address_t address);
        bool complete() const;

    private:
        const FunctionBasicBlock* basicBlockFromAddress(address_t address) const;
        FunctionBasicBlock* basicBlockFromAddress(address_t address);
        FunctionBasicBlock* getBlockAt(ListingItem *item);
        bool isStopItem(ListingItem *item) const;
        bool build(ListingItem *item);
        void buildBasicBlocks();
        void incomplete();

    private:
        std::deque<FunctionBasicBlock> m_basicblocks;
        ListingItem* m_graphstart;
        bool m_complete;
};

} // namespace REDasm
