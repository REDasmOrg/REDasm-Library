#pragma once

#include <unordered_map>
#include <list>
#include <string>
#include "../../document/document_fwd.h"
#include "../../graph/styledgraph.h"

class Disassembler;

struct FunctionBasicBlock
{
    FunctionBasicBlock(SafeDocument& document, RDGraphNode n, rd_address startaddress);
    bool contains(rd_address address) const;
    bool getStartItem(RDDocumentItem* item) const;
    bool getEndItem(RDDocumentItem* item) const;
    size_t startIndex() const;
    size_t endIndex() const;
    size_t itemsCount() const;
    const char* getStyle(RDGraphNode n) const;
    void bFalse(RDGraphNode n);
    void bTrue(RDGraphNode n);

    RDGraphNode node{0};
    rd_address startaddress, endaddress; // [startaddress, endaddress]

    private:
        std::unordered_map<RDGraphNode, std::string> m_styles;
        SafeDocument& m_document;
};

class FunctionGraph: public StyledGraph
{
    public:
        FunctionGraph(Disassembler* disassembler);
        const FunctionBasicBlock* basicBlock(rd_address address) const;
        FunctionBasicBlock* basicBlock(rd_address address);
        rd_address startAddress() const;
        size_t bytesCount() const;
        bool contains(rd_address address) const;
        bool build(rd_address address);
        bool complete() const;

    private:
        FunctionBasicBlock* requestBasicBlock(rd_address startaddress);
        bool build(RDDocumentItem* item);
        void buildBasicBlocks();

    private:
        std::list<FunctionBasicBlock> m_basicblocks;
        Disassembler* m_disassembler;
        SafeDocument& m_document;
        RDBlock m_graphstart{ };
        bool m_complete{true};
};
