#include "functiongraph_impl.h"
#include <redasm/context.h>

namespace REDasm {

FunctionBasicBlockImpl::FunctionBasicBlockImpl(Node n, const ListingItem& startitem): m_node(n), m_startitem(startitem), m_enditem(startitem) { }

FunctionGraphImpl::FunctionGraphImpl(): GraphImpl() { }

size_t FunctionGraphImpl::bytesCount() const
{
    size_t c = 0;
    const BlockContainer* blocks = r_docnew->blocks();

    for(const FunctionBasicBlock& fbb : m_basicblocks)
    {
        const BlockItem* startb = r_docnew->block(fbb.startItem().address_new);
        const BlockItem* lastb = r_docnew->block(fbb.endItem().address_new);

        for(size_t i = blocks->indexOf(startb); i <= blocks->indexOf(lastb); i++)
        {
            const BlockItem* b = blocks->at(i);
            CachedInstruction instruction = r_docnew->instruction(b->start);

            if(!instruction)
            {
                r_ctx->problem("Cannot find intruction @ " + String::hex(b->start));
                continue;
            }

            c += instruction->size;
        }
    }

    return c;
}

bool FunctionGraphImpl::contains(address_t address) const { return this->basicBlockFromAddress(address) != nullptr; }

bool FunctionGraphImpl::build(address_t address)
{
    m_graphstart = r_docnew->block(address);

    if(!m_graphstart || !m_graphstart->typeIs(BlockItemType::Code))
    {
        this->incomplete();
        return false;
    }

    this->buildBasicBlocks();
    if(this->empty()) return false;

    this->setRoot(this->getBasicBlockAt(m_graphstart)->node());
    return true;
}

bool FunctionGraphImpl::complete() const { return m_complete; }

const FunctionBasicBlock *FunctionGraphImpl::basicBlockFromAddress(address_t address) const
{
    for(const FunctionBasicBlock& fbb : m_basicblocks)
    {
        if(fbb.contains(address))
            return &fbb;
    }

    return nullptr;
}

FunctionBasicBlock *FunctionGraphImpl::basicBlockFromAddress(address_t address) { return const_cast<FunctionBasicBlock*>(static_cast<const FunctionGraphImpl*>(this)->basicBlockFromAddress(address)); }
void FunctionGraphImpl::incomplete() { m_complete = false; }

bool FunctionGraphImpl::processJump(FunctionBasicBlock* fbb, const CachedInstruction& instruction, FunctionGraphImpl::WorkList& worklist)
{
    const BlockContainer* blocks = r_docnew->blocks();
    SortedSet targets = r_disasm->getTargets(instruction->address);

    for(size_t i = 0; i < targets.size(); i++)
    {
        const Variant& target = targets.at(i);
        const BlockItem* destblock = blocks->find(target.toU64());
        if(!destblock) return false;

        FunctionBasicBlock* nextfbb = this->getBasicBlockAt(destblock);

        if(instruction->is(InstructionType::Conditional)) fbb->bTrue(nextfbb->node());
        this->newEdge(fbb->node(), nextfbb->node());
        worklist.push(destblock);
    }

    return true;
}

void FunctionGraphImpl::processJumpConditional(FunctionBasicBlock* fbb, const BlockItem* block, FunctionGraphImpl::WorkList& worklist)
{
    //ListingItem* defaultitem = r_doc->symbolItem(instruction->endAddress()); // Check for symbol first (chained jumps)
    FunctionBasicBlock* nextfbb = this->getBasicBlockAt(block);
    fbb->bFalse(nextfbb->node());
    this->newEdge(fbb->node(), nextfbb->node());
    worklist.push(block);
}

FunctionBasicBlock *FunctionGraphImpl::getBasicBlockAt(const BlockItem* block)
{
    FunctionBasicBlock* fbb = this->basicBlockFromAddress(block->start);
    if(fbb) return fbb;

    m_basicblocks.emplace_back(this->newNode(), r_docnew->itemListing(block->start));
    fbb = &m_basicblocks.back();
    this->setData(fbb->node(), fbb);
    return fbb;
}

void FunctionGraphImpl::buildBasicBlocks()
{
    VisitedItems visiteditems;
    WorkList worklist;
    worklist.push(m_graphstart);

    const BlockContainer* blocks = r_docnew->blocks();

    while(!worklist.empty())
    {
        const BlockItem* rbi = worklist.front();
        worklist.pop();
        if(visiteditems.find(rbi) != visiteditems.end()) continue;
        visiteditems.insert(rbi);

        FunctionBasicBlock* fbb = this->getBasicBlockAt(rbi);

        for(size_t i = blocks->indexOf(rbi), first = i; i < blocks->size(); i++)
        {
            const BlockItem* bi = blocks->at(i);
            if(!bi->typeIs(BlockItemType::Code)) break;

            if(i > first)
            {
                const Symbol* symbol = r_docnew->symbol(bi->start);

                if(symbol && symbol->isFunction())
                    break; // Don't overlap functions
            }

            rbi = bi;
            CachedInstruction instruction = r_docnew->instruction(bi->start);

            if(instruction->is(InstructionType::Jump))
            {
                if(!this->processJump(fbb, instruction, worklist))
                {
                    this->incomplete();
                    return;
                }

                if(instruction->is(InstructionType::Conditional) && (bi != blocks->last()))
                    this->processJumpConditional(fbb, blocks->at(i + 1), worklist);

                break;
            }

            if(instruction->is(InstructionType::Stop))
                break;
        }

        if(!rbi)
        {
            this->incomplete();
            continue;
        }

        fbb->setEndItem(r_docnew->itemListing(rbi->start));
    }
}

} // namespace REDasm
