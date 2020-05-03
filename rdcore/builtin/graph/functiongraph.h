#pragma once

#include <unordered_map>
#include <list>
#include <string>
#include "../../document/document.h"
#include "../../graph/styledgraph.h"

class Disassembler;

struct FunctionBasicBlock
{
    FunctionBasicBlock(SafeDocument& document, RDGraphNode n, address_t startaddress);
    bool contains(address_t address) const;
    bool getStartItem(RDDocumentItem* item) const;
    bool getEndItem(RDDocumentItem* item) const;
    size_t startIndex() const;
    size_t endIndex() const;
    size_t itemsCount() const;
    const char* getStyle(RDGraphNode n) const;
    void bFalse(RDGraphNode n);
    void bTrue(RDGraphNode n);

    RDGraphNode node{0};
    address_t startaddress, endaddress; // [startaddress, endaddress]

    private:
        std::unordered_map<RDGraphNode, std::string> m_styles;
        SafeDocument& m_document;
};

class FunctionGraph: public StyledGraph
{
    public:
        FunctionGraph(Disassembler* disassembler);
        const FunctionBasicBlock* basicBlock(address_t address) const;
        FunctionBasicBlock* basicBlock(address_t address);
        address_t startAddress() const;
        size_t bytesCount() const;
        bool contains(address_t address) const;
        bool build(address_t address);
        bool complete() const;

    private:
        FunctionBasicBlock* requestBasicBlock(address_t startaddress);
        bool build(RDDocumentItem* item);
        void buildBasicBlocks();

    private:
        std::list<FunctionBasicBlock> m_basicblocks;
        Disassembler* m_disassembler;
        SafeDocument& m_document;
        RDBlock m_graphstart{ };
        bool m_complete{true};
};
