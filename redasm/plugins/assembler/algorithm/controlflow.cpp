#include "controlflow.h"
#include <impl/plugins/assembler/algorithm/controlflow_impl.h>

namespace REDasm {

ControlFlowAlgorithm::ControlFlowAlgorithm(Disassembler* disassembler): Algorithm(new ControlFlowAlgorithmImpl(this, disassembler)) { }
void ControlFlowAlgorithm::addressTableState(const State *state) { Algorithm::addressTableState(state); PIMPL_P(ControlFlowAlgorithm); p->enqueueTargets(state->instruction); }
void ControlFlowAlgorithm::enqueueTarget(address_t target, const InstructionPtr &frominstruction) { this->enqueue(target); }

void ControlFlowAlgorithm::onEmulatedOperand(const Operand *op, const InstructionPtr &instruction, u64 value)
{
    // if(instruction->is(InstructionType::Branch) && op->isTarget())
    // {
    //     this->enqueueTarget(value, instruction);
    //     EXECUTE_STATE(Algorithm::BranchState, value, op->index, instruction);
    //     return;
    // }

    // Algorithm::onEmulatedOperand(op, instruction, value);
}

void ControlFlowAlgorithm::onDecoded(const InstructionPtr &instruction)
{
    Algorithm::onDecoded(instruction);

    PIMPL_P(ControlFlowAlgorithm);
    p->enqueueTargets(instruction);

    if(!instruction->is(InstructionType::Stop))
    {
        if(instruction->is(InstructionType::Jump) && !instruction->is(InstructionType::Conditional))
            return;

        this->enqueue(instruction->endAddress());
    }
}

} // namespace REDasm
