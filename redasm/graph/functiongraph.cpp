#include "functiongraph.h"
#include <impl/graph/functiongraph_impl.h>
#include "../support/utils.h"
#include "../context.h"

namespace REDasm {
namespace Graphing {

FunctionBasicBlock::FunctionBasicBlock(): m_pimpl_p(new FunctionBasicBlockImpl()) { }
FunctionBasicBlock::FunctionBasicBlock(size_t startidx): m_pimpl_p(new FunctionBasicBlockImpl(startidx)) { }
const Node &FunctionBasicBlock::node() const { PIMPL_P(const FunctionBasicBlock); return p->m_node; }
size_t FunctionBasicBlock::startIndex() const { PIMPL_P(const FunctionBasicBlock); return p->m_startidx; }
size_t FunctionBasicBlock::endIndex() const { PIMPL_P(const FunctionBasicBlock); return p->m_endidx;  }
bool FunctionBasicBlock::contains(size_t index) const { return (index >= this->startIndex()) && (index <= this->endIndex()); }
bool FunctionBasicBlock::isEmpty() const {  return this->startIndex() > this->endIndex(); }
size_t FunctionBasicBlock::count() const { return this->isEmpty() ? 0 : ((this->endIndex() - this->startIndex()) + 1); }
void FunctionBasicBlock::bTrue(const Node &n) { PIMPL_P(FunctionBasicBlock); p->m_styles[n] = "graph_edge_true"; }
void FunctionBasicBlock::bFalse(const Node &n) { PIMPL_P(FunctionBasicBlock); p->m_styles[n] = "graph_edge_false"; }

String FunctionBasicBlock::style(const Node &n) const
{
    PIMPL_P(const FunctionBasicBlock);
    auto it = p->m_styles.find(n);

    if(it == p->m_styles.end())
        return "graph_edge";

    return it->second;
}

void FunctionBasicBlock::setStartIndex(size_t idx)
{
    PIMPL_P(FunctionBasicBlock);
    p->m_startidx = idx;
}

void FunctionBasicBlock::setEndIndex(size_t idx)
{
    PIMPL_P(FunctionBasicBlock);
    p->m_endidx = idx;
}

void FunctionBasicBlock::setNode(size_t idx)
{
    PIMPL_P(FunctionBasicBlock);
    p->m_node = idx;
}

FunctionGraph::FunctionGraph(Disassembler *disassembler): GraphT<FunctionBasicBlock>(), m_disassembler(disassembler), m_document(disassembler->document()) { }
bool FunctionGraph::containsItem(size_t index) const { return this->basicBlockFromIndex(index) != nullptr; }

bool FunctionGraph::build(address_t address)
{
    const ListingItem* item = m_document->functionStart(address);

    if(item)
        return this->build(item);

    return false;
}

const FunctionBasicBlock *FunctionGraph::basicBlockFromIndex(size_t index) const
{
    for(auto& item : this->data())
    {
        if(item.second.contains(index))
            return &item.second;
    }

    return nullptr;
}

FunctionBasicBlock *FunctionGraph::basicBlockFromIndex(size_t index) { return const_cast<FunctionBasicBlock*>(static_cast<const FunctionGraph*>(this)->basicBlockFromIndex(index)); }

bool FunctionGraph::build(const ListingItem *item)
{
    if(!item || !item->is(ListingItemType::FunctionItem))
        return false;

    m_graphstart = REDasm::make_location(item->address());
    this->buildBasicBlocks();

    if(this->empty())
        return false;

    return this->connectBasicBlocks();
}

void FunctionGraph::buildBasicBlocks()
{
    size_t index = m_document->findFunction(m_graphstart);

    while(index < m_document->size())
    {
        const ListingItem* item = m_document->itemAt(index);

        if(!this->isStopItem(item))
            break;

        index++;
    }

    if(index == REDasm::npos)
        return;

    this->resetQueue();
    m_pending.push(index);

    while(!m_pending.empty())
    {
        index = m_pending.front();
        m_pending.pop();

        if((index == REDasm::npos) || (index >= m_document->size()))
            continue;

        this->buildBasicBlock(index);
    }
}

void FunctionGraph::setConnectionType(const CachedInstruction &instruction, FunctionBasicBlock *fromfbb, FunctionBasicBlock *tofbb, bool condition)
{
    if(!instruction->is(InstructionType::Conditional))
        return;

    if(condition)
        fromfbb->bTrue(tofbb->node());
    else
        fromfbb->bFalse(tofbb->node());
}

void FunctionGraph::incomplete() const { r_ctx->problem("Incomplete graph @ " + String::hex(m_graphstart.value)); }

bool FunctionGraph::isStopItem(const ListingItem *item) const
{
    switch(item->type())
    {
        case ListingItemType::FunctionItem:
        case ListingItemType::SegmentItem:
            return true;

        default:
            break;
    }

    return false;
}

bool FunctionGraph::connectBasicBlocks()
{
    for(const Node& n : this->nodes())
    {
        FunctionBasicBlock* fromfbb = this->data(n);
        ListingItem* lastitem = m_document->itemAt(this->instructionIndexFromIndex(fromfbb->endIndex()));

        if(!lastitem || !lastitem->is(ListingItemType::InstructionItem))
        {
            this->incomplete();
            continue;
        }

        CachedInstruction instruction = m_document->instruction(lastitem->address());

        if(instruction->is(InstructionType::Jump))
        {
            for(address_t target : m_disassembler->getTargets(instruction->address))
            {
                Symbol* symbol = m_document->symbol(target);

                if(!symbol || !symbol->is(SymbolType::Code))
                    continue;

                FunctionBasicBlock* tofbb = this->basicBlockFromIndex(m_document->findSymbol(target));

                if(tofbb)
                {
                    this->setConnectionType(instruction, fromfbb, tofbb, true);
                    this->newEdge(fromfbb->node(), tofbb->node());
                }
                else
                    this->incomplete();
            }

            if(instruction->is(InstructionType::Conditional))
            {
                FunctionBasicBlock* tofbb = this->basicBlockFromIndex(this->instructionIndexFromIndex(fromfbb->endIndex() + 1));

                if(tofbb)
                {
                    this->setConnectionType(instruction, fromfbb, tofbb, false);
                    this->newEdge(fromfbb->node(), tofbb->node());
                }
                else
                    this->incomplete();
            }
        }
        else if(!instruction->is(InstructionType::Stop))
        {
            FunctionBasicBlock* tofbb = this->basicBlockFromIndex(this->symbolIndexFromIndex(fromfbb->endIndex() + 1));

            if(tofbb)
                this->newEdge(fromfbb->node(), tofbb->node());
        }
    }

    return true;
}

size_t FunctionGraph::instructionIndexFromIndex(size_t idx) const
{
    ListingItem* item = m_document->itemAt(idx);

    if(item)
        return m_document->findInstruction(item->address());

    return REDasm::npos;
}

size_t FunctionGraph::symbolIndexFromIndex(size_t idx) const
{
    ListingItem* item = m_document->itemAt(idx);

    if(item)
        return m_document->findSymbol(item->address());

    return REDasm::npos;
}

void FunctionGraph::resetQueue()
{
    IndexQueue clean;
    m_pending.swap(clean);
}

void FunctionGraph::buildBasicBlock(size_t index)
{
    if(this->basicBlockFromIndex(index))
        return;

    FunctionBasicBlock fbb(index);
    ListingItem* item = nullptr;
    size_t startindex = index;

    for(size_t i = index; i < m_document->size(); i++, index++)
    {
        item = m_document->itemAt(i);

        if(this->isStopItem(item))
            break;

        if(item->is(ListingItemType::InstructionItem))
        {
            CachedInstruction instruction = m_document->instruction(item->address());

            if(instruction->is(InstructionType::Jump))
            {
                ReferenceSet targets = m_disassembler->getTargets(instruction->address);

                for(address_t target : targets)
                {
                    Symbol* symbol = m_document->symbol(target);

                    if(!symbol || !symbol->is(SymbolType::Code))
                        continue;

                    m_pending.push(m_document->findSymbol(target));
                }

                if(!targets.empty() && instruction->is(InstructionType::Conditional))
                {
                    size_t idx = m_document->findSymbol(instruction->endAddress()); // Check for symbol first (chained jumps)

                    if(idx == -1)
                        idx = m_document->findInstruction(instruction->endAddress());

                    m_pending.push(idx);
                }

                break;
            }
            else if(instruction->is(InstructionType::Stop))
                break;
        }
        else if(item->is(ListingItemType::SymbolItem))
        {
            const Symbol* symbol = m_document->symbol(item->address());

            if(symbol && symbol->is(SymbolType::Code) && !symbol->isFunction())
                m_pending.push(index);

            if(index != startindex)
                break;
        }
    }

    if(!item)
        return;

    fbb.setEndIndex(m_document->findItem(item));

    if(this->isStopItem(item) || item->is(ListingItemType::SymbolItem))
        fbb.setEndIndex(fbb.endIndex() - 1);

    if(fbb.isEmpty())
        return;

    fbb.setNode(this->newNode());
    this->setData(fbb.node(), fbb);

    if(!this->root())
        this->setRoot(fbb.node());
}

} // namespace Graphing
} // namespace REDasm
