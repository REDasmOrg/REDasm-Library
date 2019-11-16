#include "controlflow.h"
#include <impl/plugins/assembler/algorithm/controlflow_impl.h>

namespace REDasm {

ControlFlowAlgorithm::ControlFlowAlgorithm(): Algorithm(new ControlFlowAlgorithmImpl(this)) { }
void ControlFlowAlgorithm::addressTableState(const State *state) { Algorithm::addressTableState(state); PIMPL_P(ControlFlowAlgorithm); p->enqueueTargets(state->instruction); }
void ControlFlowAlgorithm::enqueueTarget(address_t target, const CachedInstruction &frominstruction) { this->enqueue(target); }

void ControlFlowAlgorithm::onDecoded(const CachedInstruction &instruction)
{
    Algorithm::onDecoded(instruction);

    PIMPL_P(ControlFlowAlgorithm);
    p->enqueueTargets(instruction);

    if(!instruction->isStop())
    {
        if(instruction->isJump() && !instruction->isConditional())
            return;

        this->enqueue(instruction->endAddress());
    }
}

} // namespace REDasm
