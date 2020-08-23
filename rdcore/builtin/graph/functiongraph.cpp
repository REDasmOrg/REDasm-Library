#include "functiongraph.h"
#include "../../document/backend/blockcontainer.h"
#include "../../support/error.h"
#include "../../disassembler.h"
#include "../../context.h"
#include <rdapi/theme.h>
#include <unordered_set>
#include <stack>

FunctionBasicBlock::FunctionBasicBlock(SafeDocument& document, RDGraphNode n, rd_address address): node(n), startaddress(address), endaddress(address), m_document(document) { }
bool FunctionBasicBlock::contains(rd_address address) const { return ((address >= startaddress) && (address <= endaddress)); }

bool FunctionBasicBlock::getStartItem(RDDocumentItem* item) const
{
    if(m_document->symbolItem(startaddress, item)) return true;
    return m_document->instructionItem(startaddress, item);
}

bool FunctionBasicBlock::getEndItem(RDDocumentItem* item) const { return m_document->instructionItem(endaddress, item); }

size_t FunctionBasicBlock::startIndex() const
{
    RDDocumentItem item;
    if(!this->getStartItem(&item)) return RD_NPOS;
    return m_document->itemIndex(&item);
}

size_t FunctionBasicBlock::endIndex() const
{
    RDDocumentItem item;
    if(!this->getEndItem(&item)) return RD_NPOS;
    return m_document->itemIndex(&item);
}

size_t FunctionBasicBlock::itemsCount() const
{
    RDDocumentItem startitem, enditem;
    if(!this->getStartItem(&startitem) || !this->getEndItem(&enditem)) return 0;

    size_t startidx = m_document->itemIndex(&startitem), endidx = m_document->itemIndex(&enditem);
    if((startidx == RD_NPOS) || (endidx == RD_NPOS)) return 0;

    return (endidx - startidx) + 1;
}

rd_type FunctionBasicBlock::getTheme(RDGraphNode n) const
{
    auto it = m_themes.find(n);
    return (it != m_themes.end()) ? it->second : Theme_Default;
}

void FunctionBasicBlock::bFalse(RDGraphNode n) { m_themes[n] = Theme_GraphEdgeFalse; }
void FunctionBasicBlock::bTrue(RDGraphNode n) { m_themes[n] = Theme_GraphEdgeTrue; }

FunctionGraph::FunctionGraph(Disassembler* disassembler): StyledGraph(), m_disassembler(disassembler), m_document(disassembler->document()) { }
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

        for(size_t i = blocks->indexOf(&startb); i <= blocks->indexOf(&endb); i++)
        {
            const RDBlock& b = blocks->at(i);
            c += BlockContainer::size(&b);
        }
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

void FunctionGraph::buildBasicBlocks(FunctionGraph::BasicBlocks& basicblocks)
{
    const DocumentNet* net = m_disassembler->net();
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
    const DocumentNet* net = m_disassembler->net();
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
