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
        FunctionBasicBlock(Node n, const ListingItem& startitem);
        Node node() const;
        const ListingItem& startItem() const;
        const ListingItem& endItem() const;
        const ListingItem& instructionStartItem() const;
        const ListingItem& instructionEndItem() const;
        size_t startIndex() const;
        size_t endIndex() const;
        size_t instructionStartIndex() const;
        size_t instructionEndIndex() const;
        size_t count() const;
        void bTrue(Node n);
        void bFalse(Node n);
        bool contains(address_t address) const;
        bool isEmpty() const;
        String style(Node n) const;
        void setStartItem(const ListingItem& item);
        void setEndItem(const ListingItem& item);
        void setInstructionStartItem(const ListingItem& item);
        void setInstructionEndItem(const ListingItem& item);
        void setNode(Node idx);
};

class FunctionGraphImpl;

class LIBREDASM_API FunctionGraph: public Graph
{
    REDASM_OBJECT(FunctionGraph)
    PIMPL_DECLARE_PRIVATE(FunctionGraph)

    public:
        FunctionGraph();
        const FunctionBasicBlock* basicBlockFromAddress(address_t address) const;
        size_t bytesCount() const;
        bool complete() const;
        bool contains(address_t address) const;
        bool build(address_t address);
};

} // namespace REDasm
