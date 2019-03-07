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

    this->buildVertices(address);

    if(this->empty())
        return false;

    return this->buildEdges();
}

bool FunctionGraph::compareEdge(const Node *n1, const Node *n2) const
{
    return static_cast<const FunctionBlock*>(n1)->startidx <
           static_cast<const FunctionBlock*>(n2)->startidx;
}

FunctionBlock *FunctionGraph::vertexFromListingIndex(s64 index) const
{
    for(auto& item : *this)
    {
       auto* v = static_cast<FunctionBlock*>(item.get());

        if(v->contains(index))
            return v;
    }

    return nullptr;
}

void FunctionGraph::buildNode(s64 index, FunctionGraph::IndexQueue &indexqueue)
{
    auto it = std::next(m_document->begin(), index);

    if(it == m_document->end())
        return;

    auto fb = std::make_unique<FunctionBlock>(index);
    ListingItem* item = it->get();

    for( ; it != m_document->end(); it++, index++)
    {
        item = it->get();

        if(this->vertexFromListingIndex(index))
        {
            fb->labelbreak = true;
            break;
        }

        if(item->is(ListingItem::SymbolItem))
        {
            if(index == fb->startidx) // Skip first label
                continue;

            const Symbol* symbol = m_document->symbol(item->address);

            if(symbol->is(SymbolTypes::Code) && !symbol->isFunction())
                indexqueue.push(index);

            fb->labelbreak = true;
            break;
        }

        if(!item->is(ListingItem::InstructionItem))
            break;

        fb->endidx = index;
        InstructionPtr instruction = m_document->instruction(item->address);

        if(instruction->is(InstructionTypes::Jump))
        {
            for(address_t target : instruction->targets)
                indexqueue.push(m_document->symbolIndex(target));

            if(instruction->is(InstructionTypes::Conditional))
                indexqueue.push(index + 1);

            break;
        }

        if(instruction->is(InstructionTypes::Stop))
            break;
    }

    this->addNode(fb.release());
}

void FunctionGraph::buildVertices(address_t startaddress)
{
    ListingItem* item = m_document->functionStart(startaddress);

    if(!item)
        return;

    m_graphstart = REDasm::make_location(item->address);

    IndexQueue queue;

    if(!item->is(ListingItem::InstructionItem) && !item->is(ListingItem::SymbolItem))
        queue.push(m_document->indexOf(item) + 1); // Skip declaration
    else
        queue.push(m_document->indexOf(item));

    while(!queue.empty())
    {
        s64 index = queue.front();
        queue.pop();

        if(index == -1)
            continue;

        item = m_document->functionStart(m_document->itemAt(index));

        if(!item || (item->address != m_graphstart) || this->vertexFromListingIndex(index))
            continue;

        this->buildNode(index, queue);
    }
}

bool FunctionGraph::buildEdges()
{
    for(auto& item : *this)
    {
        FunctionBlock* fb = static_cast<FunctionBlock*>(item.get());
        auto it = std::next(m_document->begin(), fb->startidx);
        s64 index = fb->startidx;

        if(fb->labelbreak && (fb->endidx + 1 < static_cast<s64>(m_document->length())))
        {
            FunctionBlock* block = this->vertexFromListingIndex(fb->endidx + 1);

            if(!block)
                return false;

            this->addEdge(fb, block);
        }

        for( ; (it != m_document->end()) && (index <= fb->endidx); it++, index++)
        {
            ListingItem* item = it->get();

            if(!item->is(ListingItem::InstructionItem))
                continue;

            InstructionPtr instruction = m_document->instruction(item->address);

            if(!instruction->is(InstructionTypes::Jump))
                continue;

            for(address_t target : instruction->targets)
            {
                s64 tgtindex = m_document->symbolIndex(target);
                FunctionBlock* tofb = this->vertexFromListingIndex(tgtindex);

                if(!tofb)
                    continue;

                this->addEdge(fb, tofb);

                if(instruction->is(InstructionTypes::Conditional))
                {
                    if(tofb->startidx < fb->startidx)
                    {
                        if(instruction->is(InstructionTypes::Conditional))
                            fb->bLoopConditional(tofb);
                        else
                            fb->bLoop(tofb);
                    }
                    else
                        fb->bTrue(tofb);
                }
            }

            if(instruction->is(InstructionTypes::Conditional))
            {
                FunctionBlock* todata = this->vertexFromListingIndex(index + 1);

                if(!todata)
                    continue;

                this->addEdge(fb, todata);
                fb->bFalse(todata);
            }
        }
    }

    return true;
}

} // namespace Graphing
} // namespace REDasm
