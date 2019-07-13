#include "controlflow_impl.h"
#include <redasm/disassembler/disassembler.h>
#include <redasm/context.h>

namespace REDasm {

ControlFlowAlgorithmImpl::ControlFlowAlgorithmImpl(ControlFlowAlgorithm *algorithm): AlgorithmImpl(algorithm) { }

void ControlFlowAlgorithmImpl::enqueueTargets(const CachedInstruction& instruction)
{
    PIMPL_Q(ControlFlowAlgorithm);
    ReferenceSet targets = r_disasm->getTargets(instruction->address);

    for(address_t target : targets)
        q->enqueueTarget(target, instruction);
}

} // namespace REDasm
