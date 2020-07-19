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

    for(const FunctionBasicBlock& fbb : m_basicblocks)
    {
        RDBlock startb, endb;
        if(!m_document->block(fbb.startaddress, &startb)) REDasmError("Cannot find start block", fbb.startaddress);
        if(!m_document->block(fbb.endaddress, &endb)) REDasmError("Cannot find end block", fbb.endaddress);

        const BlockContainer* blocks = m_document->blocks(fbb.startaddress);

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
    std::stack<rd_address> pending;
    pending.push(m_graphstart.address);

    while(!pending.empty()) // Prepare blocks
    {
        rd_address address = pending.top();
        const BlockContainer* blockcontainer = m_document->blocks(address);

        RDBlock block;
        if(!blockcontainer->find(address, &block))
           REDasmError("Invalid address for block", pending.top());

        pending.pop();
        if(basicblocks.count(address)) continue;

        size_t idx = blockcontainer->indexOf(&block);
        if(idx == RD_NPOS) REDasmError("Invalid index for block", pending.top());

        basicblocks[block.address] = this->createBasicBlock(block.address);

        for( ; idx < blockcontainer->size(); idx++)
        {
            const RDBlock& b = blockcontainer->at(idx);
            if(!IS_TYPE(&b, BlockType_Code)) break;

            InstructionLock instruction(CPTR(RDDocument, &m_document), b.start);

            if(!instruction || HAS_FLAG(*instruction, InstructionFlags_Stop))
                break;

            if(!IS_TYPE(*instruction, InstructionType_Jump)) continue;

            const rd_address* targets = nullptr;
            size_t c = m_disassembler->getTargets(instruction->address, &targets);

            for(size_t i = 0; i < c; i++)
            {
                rd_address target = targets[i];

                RDSymbol symbol;
                if(!m_document->symbol(target, &symbol)) continue;
                if(IS_TYPE(&symbol, SymbolType_Import)) continue;
                if(!basicblocks.count(target)) pending.push(target);
            }

            if(!HAS_FLAG(*instruction, InstructionFlags_Conditional)) break;

            rd_address nextaddress = Sugar::nextAddress(*instruction);
            if(!basicblocks.count(nextaddress)) pending.push(nextaddress);
        }
    }
}

void FunctionGraph::buildBasicBlocks()
{
    std::map<rd_address, FunctionBasicBlock*> basicblocks;
    this->buildBasicBlocks(basicblocks);

    for(auto& [address, basicblock] : basicblocks)
    {
        const BlockContainer* blockcontainer = m_document->blocks(address);
        RDBlock block;

        if(!blockcontainer->find(address, &block))
            REDasmError("Invalid block address", block.address);

        size_t idx = blockcontainer->indexOf(&block);
        if(idx == RD_NPOS) REDasmError("Invalid index for block", block.address);

        for( ; idx < blockcontainer->size(); idx++)
        {
            const RDBlock& b = blockcontainer->at(idx);
            if(!IS_TYPE(&b, BlockType_Code)) break;

            if(address != b.address)
            {
                auto it = basicblocks.find(b.address);

                if(it != basicblocks.end())
                {
                    it = basicblocks.find(b.address);
                    this->pushEdge(basicblock->node, it->second->node);
                    break;
                }
            }

            InstructionLock instruction(CPTR(RDDocument, &m_document), b.address);
            if(!instruction) break;

            basicblock->endaddress = b.address;

            if(HAS_FLAG(instruction, InstructionFlags_Stop)) break;
            if(!IS_TYPE(instruction, InstructionType_Jump)) continue;

            const rd_address* targets = nullptr;
            size_t c = m_disassembler->getTargets(instruction->address, &targets);

            for(size_t i = 0; i < c; i++)
            {
                auto it = basicblocks.find(targets[i]);
                if(it == basicblocks.end()) continue;

                if(HAS_FLAG(instruction, InstructionFlags_Conditional))
                    basicblock->bTrue(it->second->node);

                this->pushEdge(basicblock->node, it->second->node);
            }

            if(HAS_FLAG(instruction, InstructionFlags_Conditional))
            {
                auto it = basicblocks.find(Sugar::nextAddress(*instruction));
                if(it == basicblocks.end()) continue;

                basicblock->bFalse(it->second->node);
                this->pushEdge(basicblock->node, it->second->node);
            }

            break;
        }
    }
}
