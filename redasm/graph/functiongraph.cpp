#include "functiongraph.h"

namespace REDasm {
namespace Graphing {

FunctionGraph::FunctionGraph(DisassemblerAPI *disassembler): GraphT<FunctionBasicBlock>(), m_disassembler(disassembler), m_document(disassembler->document()) { }

bool FunctionGraph::build(address_t address)
{
    ListingItem* item = m_document->functionStart(address);

    if(!item)
        return false;

    m_graphstart = REDasm::make_location(item->address);
    this->buildBasicBlocks();

    if(this->empty())
        return false;

    return this->connectBasicBlocks();
}

void FunctionGraph::buildBasicBlocks()
{
    auto it = m_document->functionItem(m_graphstart);

    while(it != m_document->end())
    {
         if(!this->isStopItem(it->get()))
             break;

        it++;
    }

    if(it == m_document->end())
        return;

    s64 index = std::distance(m_document->begin(), it);
    IndexQueue pending;
    pending.push(index);

    while(!pending.empty())
    {
        index = pending.front();
        pending.pop();

        if((index < 0) || (static_cast<u64>(index) >= m_document->length()))
            continue;

        this->buildBasicBlock(index, pending);
    }
}

FunctionBasicBlock *FunctionGraph::basicBlockFromIndex(s64 index)
{
    for(auto it = m_data.begin(); it != m_data.end(); it++)
    {
        if(it->second.contains(index))
            return &it->second;
    }

    return nullptr;
}

void FunctionGraph::setConnectionType(const InstructionPtr &instruction, FunctionBasicBlock *fromfbb, FunctionBasicBlock *tofbb, bool condition)
{
    if(!instruction->is(InstructionTypes::Conditional))
        return;

    if(condition)
        fromfbb->bTrue(tofbb->node);
    else
        fromfbb->bFalse(tofbb->node);
}

void FunctionGraph::incomplete() const { REDasm::log("WARNING: Incomplete graph @ " + REDasm::hex(m_graphstart)); }

bool FunctionGraph::isStopItem(ListingItem *item)
{
    switch(item->type)
    {
        case ListingItem::FunctionItem:
        case ListingItem::SegmentItem:
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
        FunctionBasicBlock& fromfbb = m_data[n];
        ListingItem* lastitem = m_document->itemAt(this->instructionIndexFromIndex(fromfbb.endidx));

        if(!lastitem || !lastitem->is(ListingItem::InstructionItem))
        {
            this->incomplete();
            continue;
        }

        InstructionPtr instruction = m_document->instruction(lastitem->address);

        if(instruction->is(InstructionTypes::Jump))
        {
            for(address_t target : m_disassembler->getTargets(instruction->address))
            {
                FunctionBasicBlock* tofbb = this->basicBlockFromIndex(m_document->symbolIndex(target));

                if(tofbb)
                {
                    this->setConnectionType(instruction, &fromfbb, tofbb, true);
                    this->newEdge(fromfbb.node, tofbb->node);
                }
                else
                    this->incomplete();
            }

            if(instruction->is(InstructionTypes::Conditional))
            {
                FunctionBasicBlock* tofbb = this->basicBlockFromIndex(this->instructionIndexFromIndex(fromfbb.endidx + 1));

                if(tofbb)
                {
                    this->setConnectionType(instruction, &fromfbb, tofbb, false);
                    this->newEdge(fromfbb.node, tofbb->node);
                }
                else
                    this->incomplete();
            }
        }
        else if(!instruction->is(InstructionTypes::Stop))
        {
            FunctionBasicBlock* tofbb = this->basicBlockFromIndex(this->symbolIndexFromIndex(fromfbb.endidx + 1));

            if(tofbb)
                this->newEdge(fromfbb.node, tofbb->node);
        }
    }

    return true;
}

s64 FunctionGraph::instructionIndexFromIndex(s64 idx) const
{
    ListingItem* item = m_document->itemAt(idx);

    if(item)
        return m_document->instructionIndex(item->address);

    return -1;
}

s64 FunctionGraph::symbolIndexFromIndex(s64 idx) const
{
    ListingItem* item = m_document->itemAt(idx);

    if(item)
        return m_document->symbolIndex(item->address);

    return -1;
}

void FunctionGraph::buildBasicBlock(s64 index, IndexQueue& pending)
{
    if(this->basicBlockFromIndex(index))
        return;

    FunctionBasicBlock fbb(index);
    ListingItem* item = nullptr;
    s64 startindex = index;

    for(auto it = m_document->begin() + index ; it != m_document->end(); it++, index++)
    {
        item = it->get();

        if(item->address == 0x18f87)
        {
            int zzz = 0;
            zzz++;
        }

        if(this->isStopItem(item))
            break;

        if(item->is(ListingItem::InstructionItem))
        {
            InstructionPtr instruction = m_document->instruction(item->address);

            if(instruction->is(InstructionTypes::Jump))
            {
                ReferenceSet targets = m_disassembler->getTargets(instruction->address);

                for(address_t target : targets)
                    pending.push(m_document->symbolIndex(target));

                if(!targets.empty() && instruction->is(InstructionTypes::Conditional))
                {
                    s64 idx = m_document->symbolIndex(instruction->endAddress()); // Check for symbol first (chained jumps)

                    if(idx == -1)
                        idx = m_document->instructionIndex(instruction->endAddress());

                    pending.push(idx);
                }

                break;
            }
            else if(instruction->is(InstructionTypes::Stop))
                break;
        }
        else if(item->is(ListingItem::SymbolItem))
        {
            const Symbol* symbol = m_document->symbol(item->address);

            if(symbol && symbol->is(SymbolTypes::Code) && !symbol->isFunction())
                pending.push(index);

            if(index != startindex)
                break;
        }
    }

    if(!item)
        return;

    fbb.endidx = m_document->indexOf(item);

    if(this->isStopItem(item) || item->is(ListingItem::SymbolItem))
        fbb.endidx--;

    if(fbb.isEmpty())
        return;

    fbb.node = this->newNode();
    m_data[fbb.node] = fbb;

    if(!m_root)
        m_root = fbb.node;
}

} // namespace Graphing
} // namespace REDasm
