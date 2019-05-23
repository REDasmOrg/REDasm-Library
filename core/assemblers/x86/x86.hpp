#pragma once

#include "x86.h"

namespace REDasm {

template<cs_mode mode> X86Assembler<mode>::X86Assembler(): CapstoneAssemblerPlugin<CS_ARCH_X86, mode>()
{
    SET_INSTRUCTION_TYPE(X86_INS_JA, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JAE, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JB, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JBE, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JCXZ, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JECXZ, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JE, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JG, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JGE, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JL, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JLE, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JNE, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JNO, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JNP, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JNS, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JO, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JP, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JS, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_LOOP, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_LOOPE, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_LOOPNE, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_PUSH, InstructionType::Push);
    SET_INSTRUCTION_TYPE(X86_INS_PUSHAL, InstructionType::Push);
    SET_INSTRUCTION_TYPE(X86_INS_PUSHAW, InstructionType::Push);
    SET_INSTRUCTION_TYPE(X86_INS_PUSHF, InstructionType::Push);
    SET_INSTRUCTION_TYPE(X86_INS_PUSHFD, InstructionType::Push);
    SET_INSTRUCTION_TYPE(X86_INS_PUSHFQ, InstructionType::Push);
    SET_INSTRUCTION_TYPE(X86_INS_POP, InstructionType::Pop);
    SET_INSTRUCTION_TYPE(X86_INS_POPAL, InstructionType::Pop);
    SET_INSTRUCTION_TYPE(X86_INS_POPAW, InstructionType::Pop);
    SET_INSTRUCTION_TYPE(X86_INS_POPF, InstructionType::Pop);
    SET_INSTRUCTION_TYPE(X86_INS_POPFD, InstructionType::Pop);
    SET_INSTRUCTION_TYPE(X86_INS_POPFQ, InstructionType::Pop);
    SET_INSTRUCTION_TYPE(X86_INS_HLT, InstructionType::Stop);
    SET_INSTRUCTION_TYPE(X86_INS_RET, InstructionType::Stop);
    SET_INSTRUCTION_TYPE(X86_INS_NOP, InstructionType::Nop);
    SET_INSTRUCTION_TYPE(X86_INS_MOV, InstructionType::Load);
    SET_INSTRUCTION_TYPE(X86_INS_TEST, InstructionType::Compare);

    REGISTER_INSTRUCTION(X86_INS_JA, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JAE, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JB, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JBE, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JCXZ, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JECXZ, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JE, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JG, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JGE, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JL, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JLE, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JNE, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JNO, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JNP, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JNS, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JO, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JP, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JS, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_JMP, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_CALL, &X86Assembler::setBranchTarget);
    REGISTER_INSTRUCTION(X86_INS_LEA, &X86Assembler::checkLea);
    REGISTER_INSTRUCTION(X86_INS_CMP, &X86Assembler::compareOp1);
}

template<cs_mode mode> void X86Assembler<mode>::onDecoded(const InstructionPtr &instruction)
{
    CapstoneAssemblerPlugin<CS_ARCH_X86, mode>::onDecoded(instruction);

    cs_insn* insn = reinterpret_cast<cs_insn*>(instruction->meta.userdata);
    const cs_x86& x86 = insn->detail->x86;

    for(size_t i = 0; i < x86.op_count; i++)
    {
        const cs_x86_op& op = x86.operands[i];

        if(op.type == X86_OP_MEM) {
            const x86_op_mem& mem = op.mem;
            s64 locindex = -1;

            if((mem.index == X86_REG_INVALID) && mem.disp && this->isBP(mem.base)) // Check locals/arguments
            {
                OperandType type = OperandType::None;
                locindex = this->bpIndex(mem.disp, type);
                instruction->local(locindex, X86_REGISTER(mem.base), X86_REGISTER(mem.index), mem.disp, type);
            }
            else if(this->isSP(mem.base)) // Check locals
            {
                locindex = this->spIndex(mem.disp);

                if(locindex != -1)
                    instruction->local(locindex, X86_REGISTER(mem.base), X86_REGISTER(mem.index), mem.disp);
                else
                    instruction->disp(X86_REGISTER(mem.base), X86_REGISTER(mem.index), mem.scale, mem.disp);
            }
            else if((mem.index == X86_REG_INVALID) && this->isIP(mem.base)) // Handle case [xip + disp]
                instruction->mem(instruction->address + instruction->size + mem.disp);
            else if((mem.index == X86_REG_INVALID) && (mem.base == X86_REG_INVALID)) // Handle case [disp]
                instruction->mem(mem.disp);
            else
                instruction->disp(X86_REGISTER(mem.base), X86_REGISTER(mem.index), mem.scale, mem.disp);
        }
        else if(op.type == X86_OP_IMM)
            instruction->imm(op.imm);
        else if(op.type == X86_OP_REG)
            instruction->reg(op.reg);
    }
}

template<cs_mode mode> s64 X86Assembler<mode>::bpIndex(s64 disp, OperandType& type) const
{
    if(disp < 0)
    {
        type = OperandType::Local;
        return -disp;
    }

    s32 size = 0;

    if(mode == CS_MODE_16)
        size = 2;
    else if(mode == CS_MODE_32)
        size = 4;
    else if(mode == CS_MODE_64)
        size = 8;

    if(disp < (size * 2))
        return -1;

    if(disp > 0)
        type = OperandType::Argument;

    return disp;
}

template<cs_mode mode> s64 X86Assembler<mode>::spIndex(s64 disp) const
{
    if(disp <= 0)
        return -1;

    return disp;
}

template<cs_mode mode> bool X86Assembler<mode>::isSP(register_id_t reg) const
{
    if(mode == CS_MODE_16)
        return reg == X86_REG_SP;

    if(mode == CS_MODE_32)
        return reg == X86_REG_ESP;

    if(mode == CS_MODE_64)
        return reg == X86_REG_RSP;

    return false;
}

template<cs_mode mode> bool X86Assembler<mode>::isBP(register_id_t reg) const
{
    if(mode == CS_MODE_16)
        return reg == X86_REG_BP;

    if(mode == CS_MODE_32)
        return reg == X86_REG_EBP;

    if(mode == CS_MODE_64)
        return reg == X86_REG_RBP;

    return false;
}

template<cs_mode mode> bool X86Assembler<mode>::isIP(register_id_t reg) const
{
    if(mode == CS_MODE_16)
        return reg == X86_REG_IP;

    if(mode == CS_MODE_32)
        return reg == X86_REG_EIP;

    if(mode == CS_MODE_64)
        return reg == X86_REG_RIP;

    return false;
}

template<cs_mode mode> void X86Assembler<mode>::setBranchTarget(const InstructionPtr& instruction) { instruction->targetIdx(0); }

template<cs_mode mode> void X86Assembler<mode>::checkLea(const InstructionPtr &instruction)
{
    instruction->type = InstructionType::Load;
    Operand* op1 = instruction->op(1);

    if(!op1->is(OperandType::Memory))
        return;

    op1->type = OperandType::Immediate;
}

template<cs_mode mode> void X86Assembler<mode>::compareOp1(const InstructionPtr &instruction)
{
    instruction->type = InstructionType::Compare;
    Operand* op1 = instruction->op(1);
    op1->checkCharacter();
}

} // namespace REDasm


