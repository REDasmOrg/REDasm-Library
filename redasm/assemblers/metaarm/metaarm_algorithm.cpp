#include "metaarm_algorithm.h"
#include "metaarm.h"
#include "metaarm_isa.h"
#include <capstone.h>

namespace REDasm {

MetaARMAlgorithm::MetaARMAlgorithm(DisassemblerAPI *disassembler, AssemblerPlugin *assembler): ControlFlowAlgorithm(disassembler, assembler) { }

void MetaARMAlgorithm::onEmulatedOperand(const Operand &op, const InstructionPtr &instruction, u64 value)
{
    MetaARMAssembler* metaarm = static_cast<MetaARMAssembler*>(m_assembler);

    if(metaarm->isPC(op) || metaarm->isLR(op)) // Don't generate references for PC/LR registers
        return;

    ControlFlowAlgorithm::onEmulatedOperand(op, instruction, value & 0xFFFFFFFE);
}

void MetaARMAlgorithm::enqueueTarget(address_t target, const InstructionPtr &instruction)
{
    address_t ctarget = target & 0xFFFFFFFE;
    ControlFlowAlgorithm::enqueueTarget(ctarget, instruction);
}

void MetaARMAlgorithm::decodeState(State *state)
{
    MetaARMAssembler* metaarm = static_cast<MetaARMAssembler*>(m_assembler);
    int res = MetaARMAssemblerISA::classify(m_format->buffer(state->address), m_disassembler, metaarm->armAssembler());

    if(res == MetaARMAssemblerISA::Thumb)
        metaarm->switchToThumb();
    else
        metaarm->switchToArm();

    ControlFlowAlgorithm::decodeState(state);
}

void MetaARMAlgorithm::memoryState(State *state)
{
    if(state->address & 1)
        return;

    ControlFlowAlgorithm::memoryState(state);
}

void MetaARMAlgorithm::pointerState(State *state)
{
    u64 value = 0;

    if(!m_disassembler->dereference(state->address, &value))
    {
        FORWARD_STATE(AssemblerAlgorithm::ImmediateState, state);
        return;
    }

    if(value & 1)
        return;

    ControlFlowAlgorithm::pointerState(state);
}

void MetaARMAlgorithm::immediateState(State *state)
{
    if(state->u_value & 1)
        return;

    ControlFlowAlgorithm::immediateState(state);
}

} // namespace REDasm
