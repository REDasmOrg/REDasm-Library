#include "calltree_impl.h"
#include <redasm/disassembler/disassembler.h>
#include <redasm/context.h>

namespace REDasm {

CallTreeImpl::CallTreeImpl(CallTree* q): m_pimpl_q(q) { }

bool CallTreeImpl::hasCalls() const
{
    PIMPL_Q(const CallTree);
    if(!q->empty()) return true;

    auto* graph = this->graph();
    if(!graph) return false;

    const auto& nodes = graph->nodes();

    for(size_t i = 0; i < nodes.size(); i++)
    {
        Node n = nodes.at(i);
        const FunctionBasicBlock* fbb = variant_object<FunctionBasicBlock>(graph->data(n));
        if(!fbb) continue;

        const BlockItem* startblock = r_docnew->block(fbb->startItem().address_new);
        const BlockItem* endblock = r_docnew->block(fbb->endItem().address_new);
        if(!startblock || !endblock) continue;

        const auto* blocks = r_docnew->blocks();

        for(size_t j = blocks->indexOf(startblock); j <= blocks->indexOf(endblock); j++)
        {
            const BlockItem* bi = blocks->at(j);
            if(!bi->typeIs(BlockItemType::Code)) continue;

            CachedInstruction instruction = r_docnew->instruction(bi->start);
            if(instruction->is(InstructionType::Call)) return true;
        }
    }

    return false;
}

const FunctionGraph* CallTreeImpl::graph() const
{
    auto location = this->fetchLocation();
    if(!location.valid) return nullptr;
    return r_docnew->graph(location);
}

address_location CallTreeImpl::fetchLocation() const
{
    PIMPL_Q(const CallTree);
    address_t address = q->data.address_new;

    if(q->hasParent())
    {
        CachedInstruction instruction = r_docnew->instruction(address);

        if(!instruction || !instruction->typeIs(InstructionType::Call))
            return REDasm::invalid_location<address_t>();

        auto location = r_disasm->getTarget(instruction->address);
        if(!location.valid) return REDasm::invalid_location<address_t>();
        address = location;
    }

    return REDasm::make_location<address_t>(address);
}

} // namespace REDasm
