#include "functiongraph_impl.h"
#include <redasm/context.h>

namespace REDasm {

FunctionBasicBlockImpl::FunctionBasicBlockImpl(): m_startidx(REDasm::npos), m_endidx(m_startidx) { }
FunctionBasicBlockImpl::FunctionBasicBlockImpl(size_t startidx): m_startidx(startidx), m_endidx(startidx) { }

FunctionGraphImpl::FunctionGraphImpl(): GraphImpl() { }
bool FunctionGraphImpl::containsItem(size_t index) const { return this->basicBlockFromIndex(index) != nullptr; }

bool FunctionGraphImpl::build(const ListingItem *item)
{
    if(!item || !item->is(ListingItemType::FunctionItem))
        return false;

    m_graphstart = REDasm::make_location(item->address());
    this->buildBasicBlocks();

    if(this->empty())
        return false;

    this->connectBasicBlocks();
    return true;
}

bool FunctionGraphImpl::build(address_t address)
{
    const ListingItem* item = r_doc->functionStart(address);

    if(item)
        return this->build(item);

    return false;
}

const FunctionBasicBlock *FunctionGraphImpl::basicBlockFromIndex(size_t index) const
{
    for(const FunctionBasicBlock& fbb : m_basicblocks)
    {
        if(fbb.contains(index))
            return &fbb;
    }

    return nullptr;
}

FunctionBasicBlock *FunctionGraphImpl::basicBlockFromIndex(size_t index) { return const_cast<FunctionBasicBlock*>(static_cast<const FunctionGraphImpl*>(this)->basicBlockFromIndex(index)); }

void FunctionGraphImpl::buildBasicBlocks()
{
    size_t index = r_doc->findFunction(m_graphstart);

    while(index < r_doc->size())
    {
        const ListingItem* item = r_doc->itemAt(index);

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

        if((index == REDasm::npos) || (index >= r_doc->size()))
            continue;

        this->buildBasicBlock(index);
    }
}

void FunctionGraphImpl::setConnectionType(const CachedInstruction &instruction, FunctionBasicBlock *fromfbb, FunctionBasicBlock *tofbb, bool condition)
{
    if(!instruction->is(InstructionType::Conditional))
        return;

    if(condition)
        fromfbb->bTrue(tofbb->node());
    else
        fromfbb->bFalse(tofbb->node());
}

void FunctionGraphImpl::incomplete() const { r_ctx->problem("Incomplete graph @ " + String::hex(m_graphstart.value)); }

bool FunctionGraphImpl::isStopItem(const ListingItem *item) const
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

void FunctionGraphImpl::connectBasicBlocks()
{
    this->nodes().each([&](Node n) {
        FunctionBasicBlock* fromfbb = variant_object<FunctionBasicBlock>(this->data(n));
        ListingItem* lastitem = r_doc->itemAt(this->instructionIndexFromIndex(fromfbb->endIndex()));

        if(!lastitem || !lastitem->is(ListingItemType::InstructionItem))
        {
            this->incomplete();
            return;
        }

        CachedInstruction instruction = r_doc->instruction(lastitem->address());

        if(instruction->is(InstructionType::Jump))
        {
            SortedSet targets = r_disasm->getTargets(instruction->address);

            for(size_t i = 0; i < targets.size(); i++)
            {
                address_t target = targets[i].toU64();
                Symbol* symbol = r_doc->symbol(target);

                if(!symbol || !symbol->is(SymbolType::Code))
                    continue;

                FunctionBasicBlock* tofbb = this->basicBlockFromIndex(r_doc->findSymbol(target));

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
    });
}

size_t FunctionGraphImpl::instructionIndexFromIndex(size_t idx) const
{
    ListingItem* item = r_doc->itemAt(idx);

    if(item)
        return r_doc->findInstruction(item->address());

    return REDasm::npos;
}

size_t FunctionGraphImpl::symbolIndexFromIndex(size_t idx) const
{
    ListingItem* item = r_doc->itemAt(idx);

    if(item)
        return r_doc->findSymbol(item->address());

    return REDasm::npos;
}

void FunctionGraphImpl::resetQueue()
{
    IndexQueue clean;
    m_pending.swap(clean);
}

void FunctionGraphImpl::buildBasicBlock(size_t index)
{
    if(this->basicBlockFromIndex(index))
        return;

    FunctionBasicBlock fbb(index);
    ListingItem* item = nullptr;
    size_t startindex = index;

    for(size_t i = index; i < r_doc->size(); i++, index++)
    {
        item = r_doc->itemAt(i);

        if(this->isStopItem(item))
            break;

        if(item->is(ListingItemType::InstructionItem))
        {
            CachedInstruction instruction = r_doc->instruction(item->address());

            if(instruction->is(InstructionType::Jump))
            {
                SortedSet targets = r_disasm->getTargets(instruction->address);

                for(size_t i = 0; i < targets.size(); i++)
                {
                    address_t target = targets[i].toU64();
                    Symbol* symbol = r_doc->symbol(target);

                    if(!symbol || !symbol->is(SymbolType::Code))
                        continue;

                    m_pending.push(r_doc->findSymbol(target));
                }

                if(!targets.empty() && instruction->is(InstructionType::Conditional))
                {
                    size_t idx = r_doc->findSymbol(instruction->endAddress()); // Check for symbol first (chained jumps)

                    if(idx == -1)
                        idx = r_doc->findInstruction(instruction->endAddress());

                    m_pending.push(idx);
                }

                break;
            }
            else if(instruction->is(InstructionType::Stop))
                break;
        }
        else if(item->is(ListingItemType::SymbolItem))
        {
            const Symbol* symbol = r_doc->symbol(item->address());

            if(symbol && symbol->is(SymbolType::Code) && !symbol->isFunction())
                m_pending.push(index);

            if(index != startindex)
                break;
        }
    }

    if(!item)
        return;

    fbb.setEndIndex(r_doc->findItem(item));

    if(this->isStopItem(item) || item->is(ListingItemType::SymbolItem))
        fbb.setEndIndex(fbb.endIndex() - 1);

    if(fbb.isEmpty())
        return;

    fbb.setNode(this->newNode());

    m_basicblocks.push_back(fbb);
    this->setData(fbb.node(), &m_basicblocks.back());

    if(!this->root())
        this->setRoot(fbb.node());
}

} // namespace REDasm
