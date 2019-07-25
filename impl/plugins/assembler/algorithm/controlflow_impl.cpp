#include "controlflow_impl.h"
#include <redasm/disassembler/disassembler.h>
#include <redasm/context.h>

namespace REDasm {

ControlFlowAlgorithmImpl::ControlFlowAlgorithmImpl(ControlFlowAlgorithm *algorithm): AlgorithmImpl(algorithm) { }

void ControlFlowAlgorithmImpl::enqueueTargets(const CachedInstruction& instruction)
{
    PIMPL_Q(ControlFlowAlgorithm);
    SortedSet targets = r_disasm->getTargets(instruction->address);

    targets.each([&](const Variant& v) {
        q->enqueueTarget(v.toU64(), instruction);
    });
}

} // namespace REDasm
