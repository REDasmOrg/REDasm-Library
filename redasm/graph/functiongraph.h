#pragma once

#include <queue>
#include "../disassembler/disassembler.h"
#include "../disassembler/listing/listingdocument.h"
#include "graph.h"

namespace REDasm {
namespace Graphing {

class FunctionBasicBlockImpl;

class LIBREDASM_API FunctionBasicBlock
{
    PIMPL_DECLARE_P(FunctionBasicBlock)
    PIMPL_DECLARE_PRIVATE(FunctionBasicBlock)

    public:
        FunctionBasicBlock();
        FunctionBasicBlock(size_t startidx);
        const Node& node() const;
        size_t startIndex() const;
        size_t endIndex() const;
        size_t count() const;
        void bTrue(const Node& n);
        void bFalse(const Node& n);
        bool contains(size_t index) const;
        bool isEmpty() const;
        std::string style(const Node& n) const;
        void setStartIndex(size_t idx);
        void setEndIndex(size_t idx);
        void setNode(size_t idx);
};

class LIBREDASM_API FunctionGraph: public GraphT<FunctionBasicBlock>
{
    private:
        typedef std::queue<size_t> IndexQueue;

    public:
        FunctionGraph(Disassembler* disassembler);
        bool containsItem(size_t index) const;
        bool build(const ListingItem *item);
        bool build(address_t address);

    private:
        const FunctionBasicBlock* basicBlockFromIndex(size_t index) const;
        FunctionBasicBlock* basicBlockFromIndex(size_t index);
        void setConnectionType(const InstructionPtr& instruction, FunctionBasicBlock *fromfbb, FunctionBasicBlock *tofbb, bool condition);
        void incomplete() const;
        bool isStopItem(const ListingItem *item) const;
        void buildBasicBlock(size_t index);
        void buildBasicBlocks();
        bool connectBasicBlocks();

    private:
        size_t instructionIndexFromIndex(size_t idx) const;
        size_t symbolIndexFromIndex(size_t idx) const;
        void resetQueue();

    private:
        Disassembler* m_disassembler;
        ListingDocument& m_document;
        address_location m_graphstart;
        IndexQueue m_pending;
};

} // namespace Graphing
} // namespace REDasm
