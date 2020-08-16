#pragma once

#include <unordered_map>
#include <string>
#include <list>
#include <map>
#include "../../document/document_fwd.h"
#include "../../document/documentnet.h"
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
    rd_type getTheme(RDGraphNode n) const;
    void bFalse(RDGraphNode n);
    void bTrue(RDGraphNode n);

    RDGraphNode node{0};
    rd_address startaddress, endaddress; // [startaddress, endaddress]

    private:
        std::unordered_map<RDGraphNode, rd_type> m_themes;
        SafeDocument& m_document;
};

class FunctionGraph: public StyledGraph
{
    private:
        typedef std::map<rd_address, FunctionBasicBlock*> BasicBlocks;

    public:
        FunctionGraph(Disassembler* disassembler);
        const FunctionBasicBlock* basicBlock(rd_address address) const;
        FunctionBasicBlock* basicBlock(rd_address address);
        rd_address startAddress() const;
        size_t bytesCount() const;
        bool contains(rd_address address) const;
        bool build(rd_address address);
        bool complete() const;

    protected:
        std::string nodeLabel(RDGraphNode n) const override;

    private:
        bool isCode(rd_address address) const;
        FunctionBasicBlock* createBasicBlock(rd_address startaddress);
        void buildBasicBlocks(BasicBlocks& basicblocks);
        void buildBasicBlocks();

    private:
        std::list<FunctionBasicBlock> m_basicblocks;
        Disassembler* m_disassembler;
        SafeDocument& m_document;
        RDBlock m_graphstart{ };
        bool m_complete{true};
};
