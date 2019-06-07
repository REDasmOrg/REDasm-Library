#include "controlflow_impl.h"

namespace REDasm {

ControlFlowAlgorithmImpl::ControlFlowAlgorithmImpl(ControlFlowAlgorithm *algorithm, Disassembler *disassembler): AlgorithmImpl(algorithm, disassembler) { }

void ControlFlowAlgorithmImpl::enqueueTargets(const InstructionPtr &instruction)
{
    PIMPL_Q(ControlFlowAlgorithm);

    ReferenceSet targets = q->disassembler()->getTargets(instruction->address);

    for(address_t target : targets)
        q->enqueueTarget(target, instruction);
}

} // namespace REDasm
