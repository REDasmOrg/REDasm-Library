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
        FunctionBasicBlock(size_t startidx);
        Node node() const;
        size_t startIndex() const;
        size_t endIndex() const;
        size_t count() const;
        void bTrue(Node n);
        void bFalse(Node n);
        bool contains(size_t index) const;
        bool isEmpty() const;
        String style(Node n) const;
        void setStartIndex(size_t idx);
        void setEndIndex(size_t idx);
        void setNode(size_t idx);
};

class FunctionGraphImpl;

class LIBREDASM_API FunctionGraph: public Graph
{
    REDASM_OBJECT(FunctionGraph)
    PIMPL_DECLARE_PRIVATE(FunctionGraph)

    public:
        FunctionGraph();
        bool containsItem(size_t index) const;
        bool build(const ListingItem *item);
        bool build(address_t address);
};

} // namespace REDasm
