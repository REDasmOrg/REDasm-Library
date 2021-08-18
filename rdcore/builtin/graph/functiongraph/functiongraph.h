#pragma once

#include <string>
#include <map>
#include "../../../document/documentnet.h"
#include "functionbasicblock.h"

class FunctionGraph: public StyledGraph
{
    private:
        typedef std::map<rd_address, FunctionBasicBlock*> BasicBlocksPtrs;
        typedef std::map<rd_address, FunctionBasicBlock> BasicBlocks;

    public:
        FunctionGraph(Context* ctx);
        const FunctionBasicBlock* basicBlock(rd_address address) const;
        FunctionBasicBlock* basicBlock(rd_address address);
        const BasicBlocks& basicBlocks() const;
        rd_address startAddress() const;
        rd_address endAddress() const;
        size_t blocksCount() const;
        size_t bytesCount() const;
        bool contains(rd_address address) const;
        bool build(rd_address address);
        bool complete() const;

    protected:
        std::string nodeLabel(RDGraphNode n) const override;

    private:
        FunctionBasicBlock* createBasicBlock(rd_address startaddress);
        void buildBasicBlocks(BasicBlocksPtrs& basicblocks);
        void buildBasicBlocks();

    private:
        mutable size_t m_bytescount{0}, m_blockscount{0};
        mutable RDBlock m_graphend{ };
        BasicBlocks m_basicblocks;
        SafeDocument& m_document;
        RDBlock m_graphstart{ };
        bool m_complete{true};
};
