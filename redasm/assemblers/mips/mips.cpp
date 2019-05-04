#pragma once

#include "mips.h"

namespace REDasm {

template<s64 mode> MIPSAssembler<mode>::MIPSAssembler(): CapstoneAssemblerPlugin<CS_ARCH_MIPS, mode>()
{
    SET_INSTRUCTION_TYPE(MIPS_INS_NOP, InstructionType::Nop);
    SET_INSTRUCTION_TYPE(MIPS_INS_BREAK, InstructionType::Stop);
    SET_INSTRUCTION_TYPE(MIPS_INS_J, InstructionType::Jump);
    SET_INSTRUCTION_TYPE(MIPS_INS_B, InstructionType::Jump);
    SET_INSTRUCTION_TYPE(MIPS_INS_JAL, InstructionType::Call);
    SET_INSTRUCTION_TYPE(MIPS_INS_JALR, InstructionType::Call);
    SET_INSTRUCTION_TYPE(MIPS_INS_BAL, InstructionType::Call);
    SET_INSTRUCTION_TYPE(MIPS_INS_BEQZ, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BNEZ, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BNEL, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BLEZ, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BLEZC, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BLEZL, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BLTZL, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BGTZ, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BGEZ, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BGEZC, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BGEZL, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BGEZAL, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BGTZL, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BGEZALL, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BLTZ, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BNE, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BNEL, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BEQ, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BEQL, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BC1F, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BC1FL, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BC1TL, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_BBIT132, InstructionType::Conditional);
    SET_INSTRUCTION_TYPE(MIPS_INS_ADD, InstructionType::Add);
    SET_INSTRUCTION_TYPE(MIPS_INS_ADDI, InstructionType::Add);
    SET_INSTRUCTION_TYPE(MIPS_INS_ADDIU, InstructionType::Add);
    SET_INSTRUCTION_TYPE(MIPS_INS_ADDU, InstructionType::Add);
    SET_INSTRUCTION_TYPE(MIPS_INS_SUB, InstructionType::Sub);
    SET_INSTRUCTION_TYPE(MIPS_INS_SUBU, InstructionType::Sub);
    SET_INSTRUCTION_TYPE(MIPS_INS_MUL, InstructionType::Mul);
    SET_INSTRUCTION_TYPE(MIPS_INS_AND, InstructionType::And);
    SET_INSTRUCTION_TYPE(MIPS_INS_ANDI, InstructionType::And);
    SET_INSTRUCTION_TYPE(MIPS_INS_OR, InstructionType::Or);
    SET_INSTRUCTION_TYPE(MIPS_INS_ORI, InstructionType::Or);
    SET_INSTRUCTION_TYPE(MIPS_INS_XOR, InstructionType::Xor);
    SET_INSTRUCTION_TYPE(MIPS_INS_XORI, InstructionType::Xor);
    SET_INSTRUCTION_TYPE(MIPS_INS_SLL, InstructionType::Lsh);
    SET_INSTRUCTION_TYPE(MIPS_INS_SLLV, InstructionType::Lsh);
    SET_INSTRUCTION_TYPE(MIPS_INS_SRL, InstructionType::Rsh);
    SET_INSTRUCTION_TYPE(MIPS_INS_SRLV, InstructionType::Rsh);
    SET_INSTRUCTION_TYPE(MIPS_INS_SRAV, InstructionType::Rsh);

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
        instruction->type = InstructionType::Jump;
        instruction->op(0)->asTarget();
    }
    else
        instruction->type = InstructionType::Stop;
}

} // namespace REDasm
