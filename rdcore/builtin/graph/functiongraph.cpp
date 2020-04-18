#include "functiongraph.h"
#include "../../document/backend/blockcontainer.h"
#include "../../disassembler.h"
#include "../../context.h"
#include <cassert>
#include <stack>

FunctionBasicBlock::FunctionBasicBlock(SafeDocument& document, RDGraphNode n, address_t address): node(n), startaddress(address), endaddress(address), m_document(document) { }
bool FunctionBasicBlock::contains(address_t address) const { return ((address >= startaddress) && (address <= endaddress)); }
bool FunctionBasicBlock::getStartItem(RDDocumentItem* item) const { return this->getItem(startaddress, item); }
bool FunctionBasicBlock::getEndItem(RDDocumentItem* item) const { return this->getItem(endaddress, item); }

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

void FunctionBasicBlock::bFalse(RDGraphNode n) { m_styles[n] = "graph_edge_true"; }
void FunctionBasicBlock::bTrue(RDGraphNode n) { m_styles[n] = "graph_edge_false"; }

bool FunctionBasicBlock::getItem(address_t address, RDDocumentItem* item) const
{
    if(m_document->symbolItem(address, item)) return true;
    return m_document->instructionItem(address, item);
}

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

    this->setRoot(this->getBasicBlockAt(m_graphstart.start)->node);
    return true;
}

bool FunctionGraph::complete() const { return m_complete; }

FunctionBasicBlock* FunctionGraph::getBasicBlockAt(address_t startaddress)
{
    auto it = std::find_if(m_basicblocks.begin(), m_basicblocks.end(), [startaddress](const auto& fbb) {
        return fbb.startaddress == startaddress;
    });

    if(it != m_basicblocks.end()) return std::addressof(*it);

    FunctionBasicBlock& fbb = m_basicblocks.emplace_back(m_document, this->pushNode(), startaddress);
    this->setData(fbb.node, std::addressof(fbb));
    return &fbb;
}

std::optional<address_t> FunctionGraph::findNextBranch(address_t address, RDDocumentItem* item)
{
    bool found = false;
    std::optional<address_t> branchaddress;
    size_t idx = m_document->instructionIndex(address);

    for(size_t i = idx; i < m_document->itemsCount(); i++)
    {
        assert(m_document->itemAt(i, item));
        if(found) break;
        if(item->type != DocumentItemType_Instruction) continue;

        InstructionLock instruction(CPTR(RDDocument, &m_document), item->address);

        switch(instruction->type)
        {
            case InstructionType_Jump:
            case InstructionType_Stop:
                branchaddress = std::make_optional<address_t>(item->address);
                found = true;
                break;

            default: break;
        }
    }

    return branchaddress;
}

address_t FunctionGraph::findNextLabel(address_t address, RDDocumentItem* item)
{
    address_t prevaddress = address;
    size_t idx = m_document->instructionIndex(address);

    for(size_t i = idx + 1; i < m_document->itemsCount(); i++)
    {
        assert(m_document->itemAt(i, item));
        if(item->type == DocumentItemType_Symbol) break;
        prevaddress = item->address;
    }

    return prevaddress;
}

void FunctionGraph::buildBasicBlocks()
{
    std::stack<address_t> worklist;
    worklist.push(m_graphstart.start);

    while(!worklist.empty())
    {
        address_t currentaddress = worklist.top();
        worklist.pop();

        FunctionBasicBlock* fbb = this->getBasicBlockAt(currentaddress);
next:
        RDDocumentItem label, branch;
        auto prevlabeladdress = this->findNextLabel(currentaddress, &label);
        auto branchaddress = this->findNextBranch(currentaddress, &branch);

        if(label.address < branch.address)
        {
            fbb->endaddress = prevlabeladdress;
            currentaddress = label.address;

            FunctionBasicBlock* nextfbb = this->getBasicBlockAt(currentaddress);
            this->pushEdge(fbb->node, nextfbb->node);
            fbb = nextfbb;
            goto next;
        }

        fbb->endaddress = *branchaddress;

        if(!branchaddress) continue;
        InstructionLock instruction(CPTR(RDDocument, &m_document), *branchaddress);
        if(instruction->type != InstructionType_Jump) continue;

        const address_t* targets = nullptr;
        size_t c = m_disassembler->getTargets(instruction->address, &targets);

        for(size_t i = 0; i < c; i++)
        {
            RDSymbol symbol;
            if(m_document->symbol(targets[i], &symbol) && (symbol.type == SymbolType_Import)) continue;

            FunctionBasicBlock* nextfbb = this->getBasicBlockAt(targets[i]);
            fbb->bTrue(nextfbb->node);
            this->pushEdge(fbb->node, nextfbb->node);
            worklist.push(targets[i]);
        }

        if(!(instruction->flags & InstructionFlags_Conditional)) continue;

        FunctionBasicBlock* nextfbb = this->getBasicBlockAt(branch.address);
        fbb->bFalse(nextfbb->node);
        this->pushEdge(fbb->node, nextfbb->node);
        fbb = nextfbb;
        currentaddress = fbb->startaddress;
        goto next;
    }
}
