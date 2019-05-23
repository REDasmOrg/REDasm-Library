#pragma once

#include "arm_common.h"

namespace REDasm {

template<cs_arch arch, size_t mode> ARMCommonAssembler<arch, mode>::ARMCommonAssembler(): CapstoneAssemblerPlugin<arch, mode>()
{
    SET_INSTRUCTION_TYPE(ARM_INS_ADD, InstructionType::Add);
    SET_INSTRUCTION_TYPE(ARM_INS_ADC, InstructionType::Add);
    SET_INSTRUCTION_TYPE(ARM_INS_SUB, InstructionType::Sub);
    SET_INSTRUCTION_TYPE(ARM_INS_SBC, InstructionType::Sub);
    SET_INSTRUCTION_TYPE(ARM_INS_RSB, InstructionType::Sub);
    SET_INSTRUCTION_TYPE(ARM_INS_RSC, InstructionType::Sub);
    SET_INSTRUCTION_TYPE(ARM_INS_LSL, InstructionType::Lsh);
    SET_INSTRUCTION_TYPE(ARM_INS_LSR, InstructionType::Rsh);
    SET_INSTRUCTION_TYPE(ARM_INS_ASR, InstructionType::Rsh);

    REGISTER_INSTRUCTION(ARM_INS_B, &ARMCommonAssembler::checkB);
    REGISTER_INSTRUCTION(ARM_INS_BL, &ARMCommonAssembler::checkCallT0);
    REGISTER_INSTRUCTION(ARM_INS_BLX, &ARMCommonAssembler::checkCallT0);
    REGISTER_INSTRUCTION(ARM_INS_BX, &ARMCommonAssembler::checkJumpT0);

    REGISTER_INSTRUCTION(ARM_INS_LDM, &ARMCommonAssembler::checkStop);
    REGISTER_INSTRUCTION(ARM_INS_POP, &ARMCommonAssembler::checkStop);

    REGISTER_INSTRUCTION(ARM_INS_LDR, &ARMCommonAssembler::checkStop_0);
    REGISTER_INSTRUCTION(ARM_INS_MOV, &ARMCommonAssembler::checkStop_0);
}

template<cs_arch arch, size_t mode> ARMCommonAssembler<arch, mode>::~ARMCommonAssembler() { }

template<cs_arch arch, size_t mode> void ARMCommonAssembler<arch, mode>::onDecoded(const InstructionPtr &instruction)
{
    CapstoneAssemblerPlugin<arch, mode>::onDecoded(instruction);

    cs_insn* insn = reinterpret_cast<cs_insn*>(instruction->meta.userdata);
    const cs_arm& arm = insn->detail->arm;

    for(size_t i = 0; i < arm.op_count; i++)
    {
        const cs_arm_op& op = arm.operands[i];

        if(op.type == ARM_OP_MEM)
        {
            const arm_op_mem& mem = op.mem;

            if((mem.index == ARM_REG_INVALID) && ARMCommonAssembler::isPC(mem.base)) // [pc]
                instruction->mem(this->pc(instruction) + mem.disp);
            else
                instruction->disp(ARM_REGISTER(mem.base), ARM_REGISTER(mem.index), mem.scale, mem.disp);
        }
        else if(op.type == ARM_OP_REG)
            instruction->reg(op.reg);
        else if(op.type == ARM_OP_IMM)
            instruction->imm(op.imm);
    }
}

template<cs_arch arch, size_t mode> void ARMCommonAssembler<arch, mode>::checkB(const InstructionPtr &instruction) const
{
    const cs_arm& arm = reinterpret_cast<cs_insn*>(instruction->meta.userdata)->detail->arm;

    if(arm.cc != ARM_CC_AL)
        instruction->type |= InstructionType::Conditional;

    instruction->targetIdx(0);
}

template<cs_arch arch, size_t mode> void ARMCommonAssembler<arch, mode>::checkStop(const InstructionPtr &instruction) const
{
    const cs_arm& arm = reinterpret_cast<cs_insn*>(instruction->meta.userdata)->detail->arm;

    if(arm.cc != ARM_CC_AL)
        return;

    for(const Operand& op : instruction->operands)
    {
        if(!op.is(OperandType::Register) || !this->isPC(op.reg.r))
            continue;

        instruction->type = InstructionType::Stop;
        break;
    }
}

template<cs_arch arch, size_t mode> void ARMCommonAssembler<arch, mode>::checkStop_0(const InstructionPtr &instruction) const
{
    const cs_arm& arm = reinterpret_cast<cs_insn*>(instruction->meta.userdata)->detail->arm;
    instruction->op(1)->size = sizeof(u32);

    if((arm.cc == ARM_CC_AL) && this->isPC(instruction->op()))
    {
        instruction->type = InstructionType::Stop;
        return;
    }
}

template<cs_arch arch, size_t mode> void ARMCommonAssembler<arch, mode>::checkJumpT0(const InstructionPtr &instruction) const
{
    instruction->type = InstructionType::Jump;
    instruction->targetIdx(0);
}

template<cs_arch arch, size_t mode> void ARMCommonAssembler<arch, mode>::checkCallT0(const InstructionPtr &instruction) const
{
    instruction->type = InstructionType::Call;
    instruction->targetIdx(0);
}

} // namespace REDasm
