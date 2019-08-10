#pragma once

#include <redasm/graph/functiongraph.h>
#include <redasm/pimpl.h>
#include <queue>
#include "graph_impl.h"

namespace REDasm {

class FunctionBasicBlockImpl
{
    PIMPL_DECLARE_Q(FunctionBasicBlock)
    PIMPL_DECLARE_PUBLIC(FunctionBasicBlock)

    public:
        FunctionBasicBlockImpl();
        FunctionBasicBlockImpl(size_t startidx);

    private:
        Node m_node;
        size_t m_startidx, m_endidx; // [startidx, endidx]
        size_t m_startinstructionidx, m_endinstructionidx; // [startinstructionidx, endinstructionidx]
        std::unordered_map<Node, String> m_styles;
};

class FunctionGraphImpl: public GraphImpl
{
    PIMPL_DECLARE_Q(FunctionGraph)
    PIMPL_DECLARE_PUBLIC(FunctionGraph)

    private:
        typedef std::queue<size_t> IndexQueue;

    public:
        FunctionGraphImpl();
        bool containsItem(size_t index) const;
        bool build(const ListingItem *item);
        bool build(address_t address);

    private:
        const FunctionBasicBlock* basicBlockFromIndex(size_t index) const;
        FunctionBasicBlock* basicBlockFromIndex(size_t index);
        void setConnectionType(const CachedInstruction& instruction, FunctionBasicBlock *fromfbb, FunctionBasicBlock *tofbb, bool condition);
        void incomplete() const;
        bool isStopItem(const ListingItem *item) const;
        void buildBasicBlock(size_t index);
        void buildBasicBlocks();
        void connectBasicBlocks();

    private:
        size_t instructionIndexFromIndex(size_t idx) const;
        size_t symbolIndexFromIndex(size_t idx) const;
        void resetQueue();

    private:
        std::deque<FunctionBasicBlock> m_basicblocks;
        address_location m_graphstart;
        IndexQueue m_pending;
};

} // namespace REDasm
