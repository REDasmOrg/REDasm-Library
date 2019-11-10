#include "calltree.h"
#include <impl/disassembler/model/calltree_impl.h>
#include "../../disassembler/model/functiongraph.h"
#include "../../disassembler/disassembler.h"
#include "../../context.h"

namespace REDasm {

CallTree::CallTree(): Tree<ListingItem>(), m_pimpl_p(new CallTreeImpl(this)) { }
CallTree::CallTree(const ListingItem& item): Tree<ListingItem>(), m_pimpl_p(new CallTreeImpl(this)) { this->data = item; }
bool CallTree::hasCalls() const { PIMPL_P(const CallTree); return p->hasCalls(); }

size_t CallTree::populate()
{
    if(!r_disasm || r_disasm->busy()) return 0;

    PIMPL_P(CallTree);
    const auto* graph = p->graph();
    if(!graph) return 0;

    graph->nodes().each([&](const Variant& v) {
        Node n = v.toInt();
        const FunctionBasicBlock* fbb = variant_object<FunctionBasicBlock>(graph->data(n));
        if(!fbb) return;

        const BlockItem* startblock = r_docnew->block(fbb->startItem().address_new);
        const BlockItem* endblock = r_docnew->block(fbb->endItem().address_new);
        if(!startblock || !endblock) return;

        const auto* blocks = r_docnew->blocks();

        for(size_t i = blocks->indexOf(startblock); i <= blocks->indexOf(endblock); i++)
        {
            const BlockItem* bi = blocks->at(i);
            if(!bi->typeIs(BlockItemType::Code)) continue;

            CachedInstruction instruction = r_docnew->instruction(bi->start);
            if(!instruction->typeIs(InstructionType::Call)) continue;
            this->add<CallTree>(r_docnew->itemInstruction(bi->start));
        }
    });

    return this->size();
}

} // namespace REDasm
