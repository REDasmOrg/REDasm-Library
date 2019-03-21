#include "functiongraph.h"

namespace REDasm {
namespace Graphing {

FunctionGraph::FunctionGraph(ListingDocument& document): Graph(), m_document(document) { }
address_location FunctionGraph::startAddress() const { return m_graphstart; }

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

bool FunctionGraph::compareEdge(const Node *n1, const Node *n2) const
{
    return static_cast<const FunctionBasicBlock*>(n1)->startidx <
           static_cast<const FunctionBasicBlock*>(n2)->startidx;
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

    IndexQueue pending;
    pending.push(std::distance(m_document->begin(), it));

    while(!pending.empty())
    {
        s64 index = pending.front();
        pending.pop();

        if((index < 0) || (static_cast<u64>(index) >= m_document->length()))
            continue;

        this->buildBasicBlock(index, pending);
    }
}

FunctionBasicBlock *FunctionGraph::basicBlockFromIndex(s64 index) const
{
    for(auto& item : *this)
    {
       auto* v = static_cast<FunctionBasicBlock*>(item.get());

        if(v->contains(index))
            return v;
    }

    return nullptr;
}

void FunctionGraph::setConnectionType(const InstructionPtr &instruction, FunctionBasicBlock *fromfbb, FunctionBasicBlock *tofbb, bool condition)
{
    if(tofbb->startidx < fromfbb->startidx)
    {
        if(instruction->is(InstructionTypes::Conditional))
            fromfbb->bLoopConditional(tofbb);
        else
            fromfbb->bLoop(tofbb);

        return;
    }

    if(!instruction->is(InstructionTypes::Conditional))
        return;

    if(condition)
        fromfbb->bTrue(tofbb);
    else
        fromfbb->bFalse(tofbb);
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
    for(auto it = this->begin(); it != this->end(); it++)
    {
        FunctionBasicBlock* fromfbb = static_cast<FunctionBasicBlock*>(it->get());
        ListingItem* lastitem = m_document->itemAt(this->instructionIndexFromIndex(fromfbb->endidx));

        if(!lastitem || !lastitem->is(ListingItem::InstructionItem))
        {
            this->incomplete();
            continue;
        }

        InstructionPtr instruction = m_document->instruction(lastitem->address);

        if(instruction->is(InstructionTypes::Jump))
        {
            for(address_t target : instruction->targets)
            {
                FunctionBasicBlock* tofbb = this->basicBlockFromIndex(m_document->symbolIndex(target));

                if(tofbb)
                {
                    this->setConnectionType(instruction, fromfbb, tofbb, true);
                    this->addEdge(fromfbb, tofbb);
                }
                else
                    this->incomplete();
            }

            if(instruction->is(InstructionTypes::Conditional))
            {
                FunctionBasicBlock* tofbb = this->basicBlockFromIndex(this->instructionIndexFromIndex(fromfbb->endidx + 1));

                if(tofbb)
                {
                    this->setConnectionType(instruction, fromfbb, tofbb, false);
                    this->addEdge(fromfbb, tofbb);
                }
                else
                    this->incomplete();
            }
        }
        else if(!instruction->is(InstructionTypes::Stop))
        {
            FunctionBasicBlock* tofbb = this->basicBlockFromIndex(this->symbolIndexFromIndex(fromfbb->endidx + 1));

            if(tofbb)
                this->addEdge(fromfbb, tofbb);
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

    std::unique_ptr<FunctionBasicBlock> fbb = std::make_unique<FunctionBasicBlock>(index);
    ListingItem* item = nullptr;
    s64 startindex = index;

    for(auto it = m_document->begin() + index ; it != m_document->end(); it++, index++)
    {
        item = it->get();

        if(this->isStopItem(item))
            break;

        if(item->is(ListingItem::InstructionItem))
        {
            InstructionPtr instruction = m_document->instruction(item->address);

            if(instruction->is(InstructionTypes::Jump))
            {
                for(address_t target : instruction->targets)
                    pending.push(m_document->symbolIndex(target));

                if(instruction->hasTargets() && instruction->is(InstructionTypes::Conditional))
                    pending.push(m_document->instructionIndex(instruction->endAddress()));

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

    fbb->endidx = m_document->indexOf(item);

    if(this->isStopItem(item) || item->is(ListingItem::SymbolItem))
        fbb->endidx--;

    if(fbb->isEmpty())
        return;

    this->addNode(fbb.release());
}

} // namespace Graphing
} // namespace REDasm
