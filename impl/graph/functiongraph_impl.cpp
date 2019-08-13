#include "functiongraph_impl.h"
#include <redasm/context.h>

namespace REDasm {

FunctionBasicBlockImpl::FunctionBasicBlockImpl(): m_node(0), m_startitem(nullptr), m_enditem(nullptr), m_startinstructionitem(nullptr), m_endinstructionitem(nullptr) { }
FunctionBasicBlockImpl::FunctionBasicBlockImpl(Node n, ListingItem *startitem): m_node(n), m_startitem(startitem), m_enditem(startitem), m_startinstructionitem(nullptr), m_endinstructionitem(nullptr) { }

FunctionGraphImpl::FunctionGraphImpl(): GraphImpl(), m_graphstart(nullptr), m_complete(true) { }
bool FunctionGraphImpl::containsItem(ListingItem* item) const { return this->basicBlockFromItem(item) != nullptr; }

bool FunctionGraphImpl::build(ListingItem *item)
{
    if(!item || !item->is(ListingItemType::FunctionItem))
        return false;

    m_graphstart = r_doc->instructionItem(item->address());

    if(!m_graphstart)
    {
        this->incomplete();
        return false;
    }

    this->buildBasicBlocks();

    if(this->empty())
        return false;

    this->setRoot(this->getBlockAt(m_graphstart)->node());
    return true;
}

bool FunctionGraphImpl::build(address_t address)
{
    ListingItem* item = r_doc->functionStart(address);

    if(item)
        return this->build(item);

    return false;
}

bool FunctionGraphImpl::complete() const { return m_complete; }

const FunctionBasicBlock *FunctionGraphImpl::basicBlockFromItem(ListingItem* item) const
{
    if(!item)
        return nullptr;

    for(const FunctionBasicBlock& fbb : m_basicblocks)
    {
        if(fbb.contains(item))
            return &fbb;
    }

    return nullptr;
}

FunctionBasicBlock *FunctionGraphImpl::basicBlockFromItem(ListingItem* item) { return const_cast<FunctionBasicBlock*>(static_cast<const FunctionGraphImpl*>(this)->basicBlockFromItem(item)); }
void FunctionGraphImpl::incomplete() { m_complete = false; }

bool FunctionGraphImpl::isStopItem(ListingItem *item) const
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

FunctionBasicBlock *FunctionGraphImpl::getBlockAt(ListingItem* item)
{
    FunctionBasicBlock* fbb = this->basicBlockFromItem(item);

    if(fbb)
        return fbb;

    m_basicblocks.emplace_back(this->newNode(), item);
    fbb = &m_basicblocks.back();
    this->setData(fbb->node(), fbb);
    return fbb;
}

void FunctionGraphImpl::buildBasicBlocks()
{
    VisitedItems visiteditems;
    WorkList worklist;
    worklist.push(m_graphstart);

    while(!worklist.empty())
    {
        ListingItem* item = worklist.front();
        worklist.pop();

        if(visiteditems.find(item) != visiteditems.end())
            continue;

        visiteditems.insert(item);
        FunctionBasicBlock* fbb = this->getBlockAt(item);

        for( ; item; item = r_doc->next(item))
        {
            if(this->isStopItem(item))
            {
                item = r_doc->prev(item);
                break;
            }

            if(item->is(ListingItemType::InstructionItem))
            {
                CachedInstruction instruction = r_doc->instruction(item->address());

                if(fbb->instructionStartItem())
                    fbb->setInstructionStartItem(item);
                else
                    fbb->setInstructionEndItem(item);

                if(instruction->is(InstructionType::Jump))
                {
                    SortedSet targets = r_disasm->getTargets(instruction->address);

                    targets.each([&](const Variant& target) {
                        Symbol* symbol = r_doc->symbol(target.toU64());

                        if(!symbol || !symbol->is(SymbolType::Code))
                            return;

                        ListingItem* labelitem = r_doc->symbolItem(target.toU64());

                        if(!labelitem) {
                            this->incomplete();
                            return;
                        }

                        FunctionBasicBlock* nextfbb = this->getBlockAt(labelitem);

                        if(instruction->is(InstructionType::Conditional))
                            fbb->bTrue(nextfbb->node());

                        this->newEdge(fbb->node(), nextfbb->node());
                        worklist.push(labelitem);
                    });

                    if(instruction->is(InstructionType::Conditional))
                    {
                         ListingItem* defaultitem = r_doc->symbolItem(instruction->endAddress()); // Check for symbol first (chained jumps)

                        if(!defaultitem)
                            defaultitem = r_doc->instructionItem(instruction->endAddress());

                        if(defaultitem)
                        {
                            FunctionBasicBlock* nextfbb = this->getBlockAt(defaultitem);
                            fbb->bFalse(nextfbb->node());
                            this->newEdge(fbb->node(), nextfbb->node());
                            worklist.push(defaultitem);
                        }
                        else
                            this->incomplete();
                    }

                    break;
                }
                else if(instruction->is(InstructionType::Stop))
                    break;
            }
            else if(item->is(ListingItemType::SymbolItem) && (item != fbb->startItem()))
            {
                const Symbol* symbol = r_doc->symbol(item->address());

                if(symbol && symbol->is(SymbolType::Code) && !symbol->isFunction())
                {
                    FunctionBasicBlock* nextfbb = this->getBlockAt(item);
                    this->newEdge(fbb->node(), nextfbb->node());
                    worklist.push(item);
                }

                item = r_doc->prev(item);
                break;
            }
        }

        if(!item)
        {
            this->incomplete();
            continue;
        }

        if(item->is(ListingItemType::EmptyItem) || item->is(ListingItemType::SeparatorItem))
            item = r_doc->prev(item);

        fbb->setEndItem(item);
    }
}

} // namespace REDasm
