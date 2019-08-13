#pragma once

#include "../disassembler/disassembler.h"
#include "../disassembler/listing/listingdocument.h"
#include "graph.h"

namespace REDasm {

class FunctionBasicBlockImpl;

class LIBREDASM_API FunctionBasicBlock: public Object
{
    REDASM_OBJECT(FunctionBasicBlock)
    PIMPL_DECLARE_P(FunctionBasicBlock)
    PIMPL_DECLARE_PRIVATE(FunctionBasicBlock)

    public:
        FunctionBasicBlock();
        FunctionBasicBlock(Node n, ListingItem* startitem);
        Node node() const;
        ListingItem* startItem() const;
        ListingItem* endItem() const;
        ListingItem* instructionStartItem() const;
        ListingItem* instructionEndItem() const;
        size_t startIndex() const;
        size_t endIndex() const;
        size_t instructionStartIndex() const;
        size_t instructionEndIndex() const;
        size_t count() const;
        size_t instructionsCount() const;
        void bTrue(Node n);
        void bFalse(Node n);
        bool contains(ListingItem *item) const;
        bool isEmpty() const;
        String style(Node n) const;
        void setStartItem(ListingItem* item);
        void setEndItem(ListingItem* item);
        void setInstructionStartItem(ListingItem* item);
        void setInstructionEndItem(ListingItem* item);
        void setNode(Node idx);
};

class FunctionGraphImpl;

class LIBREDASM_API FunctionGraph: public Graph
{
    REDASM_OBJECT(FunctionGraph)
    PIMPL_DECLARE_PRIVATE(FunctionGraph)

    public:
        FunctionGraph();
        const FunctionBasicBlock* basicBlockFromIndex(ListingItem *item) const;
        bool complete() const;
        bool containsItem(ListingItem *item) const;
        bool build(ListingItem *item);
        bool build(address_t address);
};

} // namespace REDasm
