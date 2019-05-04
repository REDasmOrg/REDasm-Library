#include "controlflow.h"

namespace REDasm {

ControlFlowAlgorithm::ControlFlowAlgorithm(DisassemblerAPI *disassembler, AssemblerPlugin *assemblerplugin): AssemblerAlgorithm(disassembler, assemblerplugin) { }

void ControlFlowAlgorithm::addressTableState(const State *state)
{
    AssemblerAlgorithm::addressTableState(state);
    this->enqueueTargets(state->instruction);
}

void ControlFlowAlgorithm::enqueueTarget(address_t target, const InstructionPtr &frominstruction)
{
    RE_UNUSED(frominstruction);
    this->enqueue(target);
}

void ControlFlowAlgorithm::onEmulatedOperand(const Operand *op, const InstructionPtr &instruction, u64 value)
{
    if(instruction->is(InstructionType::Branch) && op->isTarget())
    {
        this->enqueueTarget(value, instruction);
        EXECUTE_STATE(AssemblerAlgorithm::BranchState, value, op->index, instruction);
        return;
    }

    AssemblerAlgorithm::onEmulatedOperand(op, instruction, value);
}

void ControlFlowAlgorithm::onDecoded(const InstructionPtr &instruction)
{
    AssemblerAlgorithm::onDecoded(instruction);
    this->enqueueTargets(instruction);

    if(!instruction->is(InstructionType::Stop))
    {
        if(instruction->is(InstructionType::Jump) && !instruction->is(InstructionType::Conditional))
            return;

        this->enqueue(instruction->endAddress());
    }
}

void ControlFlowAlgorithm::enqueueTargets(const InstructionPtr &instruction)
{
    ReferenceSet targets = m_disassembler->getTargets(instruction->address);

    for(address_t target : targets)
        this->enqueueTarget(target, instruction);
}

} // namespace REDasm
