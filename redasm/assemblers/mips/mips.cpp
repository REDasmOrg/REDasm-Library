#pragma once

#include "mips.h"

namespace REDasm {

template<s64 mode> MIPSAssembler<mode>::MIPSAssembler(): CapstoneAssemblerPlugin<CS_ARCH_MIPS, mode>()
{
    SET_INSTRUCTION_TYPE(MIPS_INS_NOP, InstructionTypes::Nop);
    SET_INSTRUCTION_TYPE(MIPS_INS_BREAK, InstructionTypes::Stop);
    SET_INSTRUCTION_TYPE(MIPS_INS_J, InstructionTypes::Jump);
    SET_INSTRUCTION_TYPE(MIPS_INS_B, InstructionTypes::Jump);
    SET_INSTRUCTION_TYPE(MIPS_INS_JAL, InstructionTypes::Call);
    SET_INSTRUCTION_TYPE(MIPS_INS_JALR, InstructionTypes::Call);
    SET_INSTRUCTION_TYPE(MIPS_INS_BAL, InstructionTypes::Call);
    SET_INSTRUCTION_TYPE(MIPS_INS_BEQZ, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BNEZ, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BNEL, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BLEZ, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BLEZC, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BLEZL, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BLTZL, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BGTZ, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BGEZ, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BGEZC, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BGEZL, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BGEZAL, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BGTZL, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BGEZALL, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BLTZ, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BNE, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BNEL, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BEQ, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BEQL, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BC1F, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BC1FL, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BC1TL, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BBIT132, InstructionTypes::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_ADD, InstructionTypes::Add);
    SET_INSTRUCTION_TYPE(MIPS_INS_ADDI, InstructionTypes::Add);
    SET_INSTRUCTION_TYPE(MIPS_INS_ADDIU, InstructionTypes::Add);
    SET_INSTRUCTION_TYPE(MIPS_INS_ADDU, InstructionTypes::Add);
    SET_INSTRUCTION_TYPE(MIPS_INS_SUB, InstructionTypes::Sub);
    SET_INSTRUCTION_TYPE(MIPS_INS_SUBU, InstructionTypes::Sub);
    SET_INSTRUCTION_TYPE(MIPS_INS_MUL, InstructionTypes::Mul);
    SET_INSTRUCTION_TYPE(MIPS_INS_AND, InstructionTypes::And);
    SET_INSTRUCTION_TYPE(MIPS_INS_ANDI, InstructionTypes::And);
    SET_INSTRUCTION_TYPE(MIPS_INS_OR, InstructionTypes::Or);
    SET_INSTRUCTION_TYPE(MIPS_INS_ORI, InstructionTypes::Or);
    SET_INSTRUCTION_TYPE(MIPS_INS_XOR, InstructionTypes::Xor);
    SET_INSTRUCTION_TYPE(MIPS_INS_XORI, InstructionTypes::Xor);
    SET_INSTRUCTION_TYPE(MIPS_INS_SLL, InstructionTypes::Lsh);
    SET_INSTRUCTION_TYPE(MIPS_INS_SLLV, InstructionTypes::Lsh);
    SET_INSTRUCTION_TYPE(MIPS_INS_SRL, InstructionTypes::Rsh);
    SET_INSTRUCTION_TYPE(MIPS_INS_SRLV, InstructionTypes::Rsh);
    SET_INSTRUCTION_TYPE(MIPS_INS_SRAV, InstructionTypes::Rsh);

    REGISTER_INSTRUCTION(MIPS_INS_JR, &MIPSAssembler::checkJr);
    REGISTER_INSTRUCTION(MIPS_INS_J, &MIPSAssembler::setTargetOp0);
    REGISTER_INSTRUCTION(MIPS_INS_JAL, &MIPSAssembler::setTargetOp0);
    REGISTER_INSTRUCTION(MIPS_INS_JALR, &MIPSAssembler::setTargetOp0);
    REGISTER_INSTRUCTION(MIPS_INS_B, &MIPSAssembler::setTargetOp0);
    REGISTER_INSTRUCTION(MIPS_INS_BAL, &MIPSAssembler::setTargetOp0);
    REGISTER_INSTRUCTION(MIPS_INS_BC1FL, &MIPSAssembler::setTargetOp0);
    REGISTER_INSTRUCTION(MIPS_INS_BC1TL, &MIPSAssembler::setTargetOp0);
    REGISTER_INSTRUCTION(MIPS_INS_BC1F, &MIPSAssembler::setTargetOp0);
    REGISTER_INSTRUCTION(MIPS_INS_BEQZ, &MIPSAssembler::setTargetOp1);
    REGISTER_INSTRUCTION(MIPS_INS_BNEZ, &MIPSAssembler::setTargetOp1);
    REGISTER_INSTRUCTION(MIPS_INS_BNEL, &MIPSAssembler::setTargetOp1);
    REGISTER_INSTRUCTION(MIPS_INS_BLEZ, &MIPSAssembler::setTargetOp1);
    REGISTER_INSTRUCTION(MIPS_INS_BLEZC, &MIPSAssembler::setTargetOp1);
    REGISTER_INSTRUCTION(MIPS_INS_BLEZL, &MIPSAssembler::setTargetOp1);
    REGISTER_INSTRUCTION(MIPS_INS_BLTZL, &MIPSAssembler::setTargetOp1);
    REGISTER_INSTRUCTION(MIPS_INS_BGTZ, &MIPSAssembler::setTargetOp1);
    REGISTER_INSTRUCTION(MIPS_INS_BGEZ, &MIPSAssembler::setTargetOp1);
    REGISTER_INSTRUCTION(MIPS_INS_BGEZC, &MIPSAssembler::setTargetOp1);
    REGISTER_INSTRUCTION(MIPS_INS_BGEZL, &MIPSAssembler::setTargetOp1);
    REGISTER_INSTRUCTION(MIPS_INS_BGEZAL, &MIPSAssembler::setTargetOp1);
    REGISTER_INSTRUCTION(MIPS_INS_BGEZALL, &MIPSAssembler::setTargetOp1);
    REGISTER_INSTRUCTION(MIPS_INS_BGTZL, &MIPSAssembler::setTargetOp1);
    REGISTER_INSTRUCTION(MIPS_INS_BLTZ, &MIPSAssembler::setTargetOp1);
    REGISTER_INSTRUCTION(MIPS_INS_BNE, &MIPSAssembler::setTargetOp2);
    REGISTER_INSTRUCTION(MIPS_INS_BNEL, &MIPSAssembler::setTargetOp2);
    REGISTER_INSTRUCTION(MIPS_INS_BEQ, &MIPSAssembler::setTargetOp2);
    REGISTER_INSTRUCTION(MIPS_INS_BEQL, &MIPSAssembler::setTargetOp2);
    REGISTER_INSTRUCTION(MIPS_INS_BBIT132, &MIPSAssembler::setTargetOp2);
}

template<s64 mode> bool MIPSAssembler<mode>::decodeInstruction(const BufferView &view, const InstructionPtr& instruction)
{
    if(CapstoneAssemblerPlugin<CS_ARCH_MIPS, mode>::decodeInstruction(view, instruction))
        return true;

    return MIPSQuirks::decode(view, instruction); // Handle COP2 instructions and more
}

template<s64 mode> void MIPSAssembler<mode>::onDecoded(const InstructionPtr& instruction)
{
    CapstoneAssemblerPlugin<CS_ARCH_MIPS, mode>::onDecoded(instruction);

    cs_insn* insn = reinterpret_cast<cs_insn*>(instruction->meta.userdata);

    if(!insn)
        return;

    const cs_mips& mips = insn->detail->mips;

    for(size_t i = 0; i < mips.op_count; i++)
    {
        const cs_mips_op& op = mips.operands[i];

        if(op.type == MIPS_OP_MEM)
            instruction->disp(op.mem.base, op.mem.disp);
        else if(op.type == MIPS_OP_REG)
            instruction->reg(op.reg);
        else if(op.type == MIPS_OP_IMM)
            instruction->imm(op.imm);
    }
}

template<s64 mode> void MIPSAssembler<mode>::checkJr(const InstructionPtr& instruction) const
{
    if(instruction->op(0)->reg.r != MIPS_REG_RA)
    {
        instruction->type = InstructionTypes::Jump;
        instruction->op(0)->asTarget();
    }
    else
        instruction->type = InstructionTypes::Stop;
}

} // namespace REDasm
