#include "functiongraph.h"
#include "../../document/backend/blockcontainer.h"
#include "../../support/sugar.h"
#include "../../disassembler.h"
#include "../../context.h"
#include <unordered_set>
#include <cassert>
#include <stack>

FunctionBasicBlock::FunctionBasicBlock(SafeDocument& document, RDGraphNode n, address_t address): node(n), startaddress(address), endaddress(address), m_document(document) { }
bool FunctionBasicBlock::contains(address_t address) const { return ((address >= startaddress) && (address <= endaddress)); }

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

const char* FunctionBasicBlock::getStyle(RDGraphNode n) const
{
    auto it = m_styles.find(n);
    return (it != m_styles.end()) ? it->second.c_str() : nullptr;
}

void FunctionBasicBlock::bFalse(RDGraphNode n) { m_styles[n] = "graph_edge_false"; }
void FunctionBasicBlock::bTrue(RDGraphNode n) { m_styles[n] = "graph_edge_true"; }

FunctionGraph::FunctionGraph(Disassembler* disassembler): StyledGraph(), m_disassembler(disassembler), m_document(disassembler->document()) { }
const FunctionBasicBlock* FunctionGraph::basicBlock(address_t address) const { return const_cast<FunctionGraph*>(this)->basicBlock(address); }

FunctionBasicBlock* FunctionGraph::basicBlock(address_t address)
{
    for(FunctionBasicBlock& fbb : m_basicblocks)
    {
        if(fbb.contains(address))
            return &fbb;
    }

    return nullptr;
}

address_t FunctionGraph::startAddress() const { return m_graphstart.start; }

size_t FunctionGraph::bytesCount() const
{
    size_t c = 0;
    const BlockContainer* blocks = m_document->blocks();

    for(const FunctionBasicBlock& fbb : m_basicblocks)
    {
        RDBlock startb, endb;
        assert(m_document->block(fbb.startaddress, &startb));
        assert(m_document->block(fbb.endaddress, &endb));

        for(size_t i = blocks->indexOf(&startb); i <= blocks->indexOf(&endb); i++)
        {
            const RDBlock& b = blocks->at(i);
            InstructionLock instruction(CPTR(RDDocument, &m_document), b.start);

            if(!instruction)
            {
                rd_ctx->problem("Cannot find intruction @ " + Utils::hex(b.start));
                continue;
            }

            c += instruction->size;
        }
    }

    return c;
}

bool FunctionGraph::contains(address_t address) const
{
    for(const FunctionBasicBlock& fbb : m_basicblocks)
    {
        if(fbb.contains(address))
            return true;
    }

    return false;
}

bool FunctionGraph::build(address_t address)
{
    if(!m_document->block(address, &m_graphstart) || (m_graphstart.type != BlockType_Code))
        return false;

    this->buildBasicBlocks();
    if(this->empty()) return false;

    this->setRoot(this->basicBlock(m_graphstart.start)->node);
    return true;
}

bool FunctionGraph::complete() const { return m_complete; }

FunctionBasicBlock* FunctionGraph::requestBasicBlock(address_t startaddress)
{
    FunctionBasicBlock* fbb = this->basicBlock(startaddress);

    if(fbb)
    {
        if(fbb->startaddress == startaddress) return fbb;

        // Split and link 'fbb'
        const BlockContainer* blockcontainer = m_document->blocks();
        FunctionBasicBlock& splitfbb = m_basicblocks.emplace_back(m_document, this->pushNode(), startaddress);
        splitfbb.endaddress = fbb->endaddress;

        RDBlock block;
        assert(blockcontainer->find(startaddress, &block));

        size_t idx = blockcontainer->indexOf(&block);
        assert(idx && (idx != RD_NPOS));

        block = blockcontainer->at(--idx);
        fbb->endaddress = block.start;

        this->pushEdge(fbb->node, splitfbb.node);
        this->setData(splitfbb.node, std::addressof(splitfbb));
        return std::addressof(splitfbb);
    }

    FunctionBasicBlock& newfbb = m_basicblocks.emplace_back(m_document, this->pushNode(), startaddress);
    this->setData(newfbb.node, std::addressof(newfbb));
    return &newfbb;
}

void FunctionGraph::buildBasicBlocks()
{
    const BlockContainer* blockcontainer = m_document->blocks();

    std::unordered_set<RDBlock> doneblocks;
    std::stack<RDBlock> blocks;
    blocks.push(m_graphstart);

    while(!blocks.empty())
    {
        RDBlock block = blocks.top();
        blocks.pop();

        if(doneblocks.find(block) != doneblocks.end()) continue;
        doneblocks.insert(block);

        size_t idx = blockcontainer->indexOf(&block);
        assert(idx != RD_NPOS);

        FunctionBasicBlock* fbb = this->requestBasicBlock(block.start);
        address_t endaddress = block.start;

        for(size_t i = idx; i < blockcontainer->size(); i++)
        {
            const RDBlock& b = blockcontainer->at(i);
            if(b.type != BlockType_Code) break;

            InstructionLock instruction(CPTR(RDDocument, &m_document), b.start);

            if(!instruction || (instruction->type == InstructionType_Stop))
            {
                endaddress = b.start;
                break;
            }

            if(instruction->type != InstructionType_Jump)
            {
                endaddress = b.start;
                continue;
            }

            const address_t* targets = nullptr;
            size_t c = m_disassembler->getTargets(instruction->address, &targets);

            for(size_t i = 0; i < c; i++)
            {
                address_t target = targets[i];

                RDSymbol symbol;
                if(m_document->symbol(target, &symbol) && (symbol.type == SymbolType_Import)) continue;

                FunctionBasicBlock* nextfbb = this->requestBasicBlock(target);
                fbb->bTrue(nextfbb->node);
                this->pushEdge(fbb->node, nextfbb->node);

                RDBlock tgtblock;
                if(!blockcontainer->find(target, &tgtblock)) continue;
                if(target > fbb->startaddress) blocks.push(tgtblock);
            }

            if(instruction->flags & InstructionFlags_Conditional)
            {
                address_t nextaddress = Sugar::endAddress(*instruction);

                FunctionBasicBlock* nextfbb = this->requestBasicBlock(nextaddress);
                fbb->bFalse(nextfbb->node);
                this->pushEdge(fbb->node, nextfbb->node);

                if((i + 1) < blockcontainer->size())
                    blocks.push(blockcontainer->at(i + 1));
            }

            endaddress = b.start;
            break;
        }

        fbb->endaddress = endaddress;
    }
}
