#include "functiongraph.h"
#include "../../../document/model/blockcontainer.h"
#include "../../../support/error.h"
#include "../../../support/utils.h"
#include "../../../document/document.h"
#include "../../../context.h"
#include <stack>

FunctionGraph::FunctionGraph(Context* ctx): StyledGraph(ctx), m_document(ctx->document()) { }
const FunctionBasicBlock* FunctionGraph::basicBlock(rd_address address) const { return const_cast<FunctionGraph*>(this)->basicBlock(address); }

FunctionBasicBlock* FunctionGraph::basicBlock(rd_address address)
{
    auto it = m_basicblocks.lower_bound(address);

    if(it == m_basicblocks.end())
    {
        if(m_basicblocks.size() == 1) it = m_basicblocks.begin();
        else return nullptr;
    }

    while(!it->second.contains(address))
    {
        if(it == m_basicblocks.begin()) return nullptr;
        it--;
    }

    return std::addressof(it->second);
}

const FunctionGraph::BasicBlocks& FunctionGraph::basicBlocks() const { return m_basicblocks; }
rd_address FunctionGraph::startAddress() const { return m_graphstart.address; }

rd_address FunctionGraph::endAddress() const
{
    if(!m_blockscount) this->blocksCount();
    return m_graphend.address;
}

size_t FunctionGraph::blocksCount() const
{
    if(m_blockscount) return m_blockscount;

    for(const auto& [_, fbb] : m_basicblocks)
    {
        RDBlock startb, endb;
        if(!m_document->addressToBlock(fbb.startaddress, &startb)) REDasmError("Cannot find start block", fbb.startaddress);
        if(!m_document->addressToBlock(fbb.endaddress, &endb)) REDasmError("Cannot find end block", fbb.endaddress);

        const BlockContainer* blocks = m_document->getBlocks(fbb.startaddress);

        blocks->range(startb, endb, [&](const RDBlock& block) {
            if(IS_TYPE(&block, BlockType_Code)) m_blockscount++;
            if(block.address > m_graphend.address) m_graphend = block;
            return true;
        });
    }

    return m_blockscount;
}

size_t FunctionGraph::bytesCount() const
{
    if(m_bytescount) return m_bytescount;

    for(const auto& [_, fbb] : m_basicblocks)
    {
        RDBlock startb, endb;
        if(!m_document->addressToBlock(fbb.startaddress, &startb)) REDasmError("Cannot find start block", fbb.startaddress);
        if(!m_document->addressToBlock(fbb.endaddress, &endb)) REDasmError("Cannot find end block", fbb.endaddress);

        const BlockContainer* blocks = m_document->getBlocks(fbb.startaddress);

        blocks->range(startb, endb, [&](const RDBlock& block) {
            m_bytescount += BlockContainer::size(&block);
            return true;
        });
    }

    return m_bytescount;
}

bool FunctionGraph::contains(rd_address address) const { return this->basicBlock(address) != nullptr; }

bool FunctionGraph::build(rd_address address)
{
    m_blockscount = m_bytescount = 0;

    if(!m_document->addressToBlock(address, &m_graphstart))
    {
        spdlog::warn("FunctionGraph::build({:x}): Block not found", address);
        return false;
    }

    if(!IS_TYPE(&m_graphstart, BlockType_Code))
    {
        spdlog::warn("FunctionGraph::build({:x}): Block type is #", address, m_graphstart.type);
        return false;
    }

    this->buildBasicBlocks();

    if(this->empty())
    {
        spdlog::warn("FunctionGraph::build({:x}): Graph is empty", address);
        return false;
    }

    FunctionBasicBlock* fbb = this->basicBlock(m_graphstart.address);

    if(!fbb)
    {
        spdlog::error("FunctionGraph::build({:x}): Invalid Root Basic Block", address);
        return false;
    }

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
    auto [it, _] = m_basicblocks.emplace(startaddress, FunctionBasicBlock(m_document, this->pushNode(), startaddress));
    this->setData(it->second.node, std::addressof(it->second));
    return std::addressof(it->second);
}

void FunctionGraph::buildBasicBlocks(FunctionGraph::BasicBlocksPtrs& basicblocks)
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

        if(!link)
        {
            this->context()->problem("Cannot find node @ " + Utils::hex(address));
            continue;
        }

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
    BasicBlocksPtrs basicblocks;
    this->buildBasicBlocks(basicblocks);

    for(auto& [bbaddress, basicblock] : basicblocks)
    {
        rd_address address = bbaddress;
        auto* link = net->findNode(address);

        if(!link)
        {
            this->context()->problem("Cannot find node @ " + Utils::hex(bbaddress));
            continue;
        }

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
