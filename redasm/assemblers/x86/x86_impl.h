#include "x86.h"

namespace REDasm {

template<cs_mode mode> X86Assembler<mode>::X86Assembler(): CapstoneAssemblerPlugin<CS_ARCH_X86, mode>()
{
    SET_INSTRUCTION_TYPE(X86_INS_JA, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JAE, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JB, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JBE, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JCXZ, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JECXZ, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JE, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JG, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JGE, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JL, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JLE, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JNE, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JNO, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JNP, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JNS, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JO, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JP, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_JS, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_LOOP, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_LOOPE, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_LOOPNE, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(X86_INS_PUSH, InstructionTypes::Push);
    SET_INSTRUCTION_TYPE(X86_INS_PUSHAL, InstructionTypes::Push);
    SET_INSTRUCTION_TYPE(X86_INS_PUSHAW, InstructionTypes::Push);
    SET_INSTRUCTION_TYPE(X86_INS_PUSHF, InstructionTypes::Push);
    SET_INSTRUCTION_TYPE(X86_INS_PUSHFD, InstructionTypes::Push);
    SET_INSTRUCTION_TYPE(X86_INS_PUSHFQ, InstructionTypes::Push);
    SET_INSTRUCTION_TYPE(X86_INS_POP, InstructionTypes::Pop);
    SET_INSTRUCTION_TYPE(X86_INS_POPAL, InstructionTypes::Pop);
    SET_INSTRUCTION_TYPE(X86_INS_POPAW, InstructionTypes::Pop);
    SET_INSTRUCTION_TYPE(X86_INS_POPF, InstructionTypes::Pop);
    SET_INSTRUCTION_TYPE(X86_INS_POPFD, InstructionTypes::Pop);
    SET_INSTRUCTION_TYPE(X86_INS_POPFQ, InstructionTypes::Pop);
    SET_INSTRUCTION_TYPE(X86_INS_HLT, InstructionTypes::Stop);
    SET_INSTRUCTION_TYPE(X86_INS_RET, InstructionTypes::Stop);
    SET_INSTRUCTION_TYPE(X86_INS_NOP, InstructionTypes::Nop);
    SET_INSTRUCTION_TYPE(X86_INS_MOV, InstructionTypes::Load);
    SET_INSTRUCTION_TYPE(X86_INS_TEST, InstructionTypes::Compare);

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
                u32 type = 0;
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

template<cs_mode mode> s64 X86Assembler<mode>::bpIndex(s64 disp, u32& type) const
{
    if(disp < 0)
    {
        type = OperandTypes::Local;
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
        type = OperandTypes::Argument;

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
    instruction->type = InstructionTypes::Load;
    Operand* op1 = instruction->op(1);

    if(!op1->is(OperandTypes::Memory))
        return;

    op1->type = OperandTypes::Immediate;
}

template<cs_mode mode> void X86Assembler<mode>::compareOp1(const InstructionPtr &instruction)
{
    instruction->type = InstructionTypes::Compare;
    Operand* op1 = instruction->op(1);
    op1->checkCharacter();
}

} // namespace REDasm


