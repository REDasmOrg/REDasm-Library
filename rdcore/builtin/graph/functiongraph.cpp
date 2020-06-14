#include "functiongraph.h"
#include "../../document/backend/blockcontainer.h"
#include "../../support/error.h"
#include "../../support/sugar.h"
#include "../../disassembler.h"
#include "../../context.h"
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

const char* FunctionBasicBlock::getStyle(RDGraphNode n) const
{
    auto it = m_styles.find(n);
    return (it != m_styles.end()) ? it->second.c_str() : nullptr;
}

void FunctionBasicBlock::bFalse(RDGraphNode n) { m_styles[n] = "graph_edge_false"; }
void FunctionBasicBlock::bTrue(RDGraphNode n) { m_styles[n] = "graph_edge_true"; }

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
    const BlockContainer* blocks = m_document->blocks();

    for(const FunctionBasicBlock& fbb : m_basicblocks)
    {
        RDBlock startb, endb;
        if(!m_document->block(fbb.startaddress, &startb)) REDasmError("Cannot find start block", fbb.startaddress);
        if(!m_document->block(fbb.endaddress, &endb)) REDasmError("Cannot find end block", fbb.endaddress);

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

    FunctionBasicBlock* fbb = this->basicBlock(m_graphstart.start);
    if(!fbb) return false;

    this->setRoot(fbb->node);
    return true;
}

bool FunctionGraph::complete() const { return m_complete; }

FunctionBasicBlock* FunctionGraph::requestBasicBlock(rd_address startaddress)
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
        if(!blockcontainer->find(startaddress, &block)) REDasmError("Cannot find block", startaddress);

        size_t idx = blockcontainer->indexOf(&block);
        if(idx == RD_NPOS) REDasmError("Invalid index for block", block.address);

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
        if(idx == RD_NPOS) REDasmError("Invalid index for block", block.address);

        FunctionBasicBlock* fbb = this->requestBasicBlock(block.start);
        rd_address endaddress = block.start;

        for(size_t i = idx; i < blockcontainer->size(); i++)
        {
            const RDBlock& b = blockcontainer->at(i);
            if(!IS_TYPE(&b, BlockType_Code)) break;

            InstructionLock instruction(CPTR(RDDocument, &m_document), b.start);

            if(!instruction || HAS_FLAG(*instruction, InstructionFlags_Stop))
            {
                endaddress = b.start;
                break;
            }

            if(!IS_TYPE(*instruction, InstructionType_Jump))
            {
                endaddress = b.start;
                continue;
            }

            const rd_address* targets = nullptr;
            size_t c = m_disassembler->getTargets(instruction->address, &targets);

            for(size_t i = 0; i < c; i++)
            {
                rd_address target = targets[i];

                RDSymbol symbol;
                if(m_document->symbol(target, &symbol) && IS_TYPE(&symbol, SymbolType_Import)) continue;

                FunctionBasicBlock* nextfbb = this->requestBasicBlock(target);
                fbb->bTrue(nextfbb->node);
                this->pushEdge(fbb->node, nextfbb->node);

                RDBlock tgtblock;
                if(!blockcontainer->find(target, &tgtblock)) continue;
                if(target > fbb->startaddress) blocks.push(tgtblock);
            }

            if(HAS_FLAG(*instruction, InstructionFlags_Conditional))
            {
                rd_address nextaddress = Sugar::nextAddress(*instruction);

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
