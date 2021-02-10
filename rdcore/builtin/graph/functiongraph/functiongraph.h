#pragma once

#include <string>
#include <list>
#include <map>
#include "../../../document/documentnet.h"
#include "functionbasicblock.h"

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
        size_t instructionsCount() const;
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
        mutable size_t m_bytescount{0}, m_instructionscount{0};
        BasicBlocks m_basicblocks;
        SafeDocument& m_document;
        RDBlock m_graphstart{ };
        bool m_complete{true};
};
