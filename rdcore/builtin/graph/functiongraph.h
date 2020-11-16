#pragma once

#include <string>
#include <list>
#include <map>
#include "../../document/document_fwd.h"
#include "../../document/documentnet.h"
#include "../../graph/styledgraph.h"

struct FunctionBasicBlock
{
    FunctionBasicBlock(SafeDocument& document, RDGraphNode n, rd_address startaddress);
    bool contains(rd_address address) const;
    bool getStartItem(RDDocumentItem* item) const;
    bool getEndItem(RDDocumentItem* item) const;
    size_t itemsCount() const;
    rd_type getTheme(RDGraphNode n) const;
    void bFalse(RDGraphNode n);
    void bTrue(RDGraphNode n);

    RDGraphNode node{0};
    rd_address startaddress, endaddress; // [startaddress, endaddress]

    private:
        mutable size_t m_itemscount{0};
        std::unordered_map<RDGraphNode, rd_type> m_themes;
        SafeDocument& m_document;
};

class FunctionGraph: public StyledGraph
{
    private:
        typedef std::map<rd_address, FunctionBasicBlock*> BasicBlockMap;

    public:
        typedef std::list<FunctionBasicBlock> BasicBlocks;

    public:
        FunctionGraph(Context* ctx);
        const FunctionBasicBlock* basicBlock(rd_address address) const;
        FunctionBasicBlock* basicBlock(rd_address address);
        const BasicBlocks& basicBlocks() const;
        rd_address startAddress() const;
        size_t bytesCount() const;
        bool contains(rd_address address) const;
        bool build(rd_address address);
        bool complete() const;

    protected:
        std::string nodeLabel(RDGraphNode n) const override;

    private:
        FunctionBasicBlock* createBasicBlock(rd_address startaddress);
        void buildBasicBlocks(BasicBlockMap& basicblocks);
        void buildBasicBlocks();

    private:
        BasicBlocks m_basicblocks;
        SafeDocument& m_document;
        RDBlock m_graphstart{ };
        bool m_complete{true};
};
