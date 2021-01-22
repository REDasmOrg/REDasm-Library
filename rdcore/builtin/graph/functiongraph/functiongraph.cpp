#include "functiongraph.h"
#include "../../../document/backend/blockcontainer.h"
#include "../../../support/error.h"
#include "../../../support/utils.h"
#include "../../../document/document.h"
#include "../../../context.h"
#include <stack>

FunctionGraph::FunctionGraph(Context* ctx): StyledGraph(ctx), m_document(ctx->document()) { }
const FunctionBasicBlock* FunctionGraph::basicBlock(rd_address address) const { return const_cast<FunctionGraph*>(this)->basicBlock(address); }

FunctionBasicBlock* FunctionGraph::basicBlock(rd_address address)
{
    for(FunctionBasicBlock& fbb : m_basicblocks)
    {
        if(fbb.contains(address))
            return &fbb;
    }

    return nullptr;
}

const FunctionGraph::BasicBlocks& FunctionGraph::basicBlocks() const { return m_basicblocks; }
rd_address FunctionGraph::startAddress() const { return m_graphstart.start; }

size_t FunctionGraph::bytesCount() const
{
    size_t c = 0;

    for(const FunctionBasicBlock& fbb : m_basicblocks)
    {
        RDBlock startb, endb;
        if(!m_document->block(fbb.startaddress, &startb)) REDasmError("Cannot find start block", fbb.startaddress);
        if(!m_document->block(fbb.endaddress, &endb)) REDasmError("Cannot find end block", fbb.endaddress);

        const BlockContainer* blocks = m_document->blocks(fbb.startaddress);

        blocks->range(startb, endb, [&](const RDBlock& block) {
            c += BlockContainer::size(&block);
            return true;
        });
    }

    return c;
}

bool FunctionGraph::contains(rd_address address) const
{
    for(const FunctionBasicBlock& fbb : m_basicblocks)
    {
        if(fbb.contains(address))
            return true;
    }

    return false;
}

bool FunctionGraph::build(rd_address address)
{
    if(!m_document->block(address, &m_graphstart) || !IS_TYPE(&m_graphstart, BlockType_Code))
        return false;

    this->buildBasicBlocks();
    if(this->empty()) return false;

    FunctionBasicBlock* fbb = this->basicBlock(m_graphstart.address);
    if(!fbb) return false;

    this->setRoot(fbb->node);
    return true;
}

bool FunctionGraph::complete() const { return m_complete; }

std::string FunctionGraph::nodeLabel(RDGraphNode n) const
{
    auto* fbb = reinterpret_cast<FunctionBasicBlock*>(this->data(n)->p_data);
    if(fbb) return Utils::hex(fbb->startaddress);
    return Graph::nodeLabel(n);
}

FunctionBasicBlock* FunctionGraph::createBasicBlock(rd_address startaddress)
{
    FunctionBasicBlock& newfbb = m_basicblocks.emplace_back(m_document, this->pushNode(), startaddress);
    this->setData(newfbb.node, std::addressof(newfbb));
    return &newfbb;
}

void FunctionGraph::buildBasicBlocks(FunctionGraph::BasicBlockMap& basicblocks)
{
    const DocumentNet* net = this->context()->net();
    std::stack<rd_address> pending;
    pending.push(m_graphstart.address);

    while(!pending.empty()) // Prepare blocks
    {
        rd_address address = pending.top();
        pending.pop();

        if(!Utils::isCode(m_document, address) || basicblocks.count(address)) continue;

        const auto* link = net->findNode(address);
        if(!link) continue;

        basicblocks[address] = this->createBasicBlock(address);

        while(link)
        {
            std::for_each(link->branchestrue.begin(), link->branchestrue.end(), [&](rd_address jump) {
                if(Utils::isCode(m_document, jump)) pending.push(jump);
            });

            std::for_each(link->branchesfalse.begin(), link->branchesfalse.end(), [&](rd_address jump) {
                if(Utils::isCode(m_document, jump)) pending.push(jump);
            });

            address = link->next;
            link = net->findNode(address);
        }
    }
}

void FunctionGraph::buildBasicBlocks()
{
    const DocumentNet* net = this->context()->net();
    std::map<rd_address, FunctionBasicBlock*> basicblocks;
    this->buildBasicBlocks(basicblocks);

    for(auto& [bbaddress, basicblock] : basicblocks)
    {
        rd_address address = bbaddress;
        auto* link = net->findNode(address);
        if(!link) continue;

        while(link && Utils::isCode(m_document, address))
        {
            basicblock->endaddress = address;
            auto it = basicblocks.end();

            for(rd_address jmpaddress : link->branchestrue)
            {
                it = basicblocks.find(jmpaddress);
                if(it == basicblocks.end()) continue;

                this->pushEdge(basicblock->node, it->second->node);
                basicblock->bTrue(it->second->node);
            }

            for(rd_address jmpaddress : link->branchesfalse)
            {
                it = basicblocks.find(jmpaddress);
                if(it == basicblocks.end()) continue;

                this->pushEdge(basicblock->node, it->second->node);
                basicblock->bFalse(it->second->node);
            }

            it = basicblocks.find(link->next);

            if(it != basicblocks.end()) // Connect the block only
            {
                this->pushEdge(basicblock->node, it->second->node);
                break;
            }

            address = link->next;
            link = net->findNode(address);
        }
    }
}
