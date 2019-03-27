#include "metaarm_emulator.h"
#include "metaarm.h"
#include <capstone.h>

namespace REDasm {

MetaARMEmulator::MetaARMEmulator(DisassemblerAPI *disassembler): EmulatorT<u32>(disassembler)
{
    EMULATE_INSTRUCTION(ARM_INS_ADD, &MetaARMEmulator::emulateMath);
    EMULATE_INSTRUCTION(ARM_INS_ADC, &MetaARMEmulator::emulateMath);
    EMULATE_INSTRUCTION(ARM_INS_SUB, &MetaARMEmulator::emulateMath);
    EMULATE_INSTRUCTION(ARM_INS_SBC, &MetaARMEmulator::emulateMath);
    EMULATE_INSTRUCTION(ARM_INS_RSB, &MetaARMEmulator::emulateMath);
    ////EMULATE_INSTRUCTION(ARM_INS_RSC, &MetaARMEmulator::emulateMath);
    EMULATE_INSTRUCTION(ARM_INS_LSL, &MetaARMEmulator::emulateMath);
    EMULATE_INSTRUCTION(ARM_INS_LSR, &MetaARMEmulator::emulateMath);
    EMULATE_INSTRUCTION(ARM_INS_ASR, &MetaARMEmulator::emulateMath);

    EMULATE_INSTRUCTION(ARM_INS_MOV, &MetaARMEmulator::emulateMov);
    EMULATE_INSTRUCTION(ARM_INS_LDR, &MetaARMEmulator::emulateLdr);
    EMULATE_INSTRUCTION(ARM_INS_STR, &MetaARMEmulator::emulateStr);
}

void MetaARMEmulator::emulate(const InstructionPtr &instruction)
{
    auto* arm = dynamic_cast<ARMAbstractAssembler*>(m_disassembler->assembler());
    this->writeReg(ARM_REG_PC, static_cast<u32>(arm->pc(instruction)));
    EmulatorT<u32>::emulate(instruction);
}

bool MetaARMEmulator::setTarget(const InstructionPtr &instruction)
{
    auto* metaarm = static_cast<MetaARMAssembler*>(m_disassembler->assembler());

    if(metaarm->isPC(instruction->target()) || metaarm->isLR(instruction->target()))
        return false;

    if(!EmulatorT<u32>::setTarget(instruction))
        return false;

    address_t target = m_disassembler->getTarget(instruction->address);
    m_disassembler->popTarget(target, instruction->address);
    m_disassembler->pushTarget(target & 0xFFFFFFFE, instruction->address);
    return true;
}

void MetaARMEmulator::emulateMath(const InstructionPtr &instruction)
{
    if(instruction->id != ARM_INS_RSB)
        this->aluOp(instruction, 0, 1, 2);
    else
        this->aluOp(instruction, 0, 2, 1);

    if((instruction->id == ARM_INS_ADC) && this->hasCarry())
        this->changeReg(instruction->op(0), 1);
    else if((instruction->id == ARM_INS_SBC) && !this->hasCarry())
        this->changeReg(instruction->op(0), -1);
}

void MetaARMEmulator::emulateMov(const InstructionPtr &instruction) { this->moveOp(instruction, 0, 1); }

void MetaARMEmulator::emulateLdr(const InstructionPtr &instruction)
{
    u32 memvalue = 0;
    const Operand* op2 = instruction->op(1);

    if(!op2->is(OperandTypes::Memory) || !this->readOp(op2, &memvalue))
        return;

    this->writeOp(instruction->op(0), memvalue);
}

void MetaARMEmulator::emulateStr(const InstructionPtr &instruction)
{
    u32 regvalue = 0;

    if(!this->readOp(instruction->op(0), &regvalue))
        return;

    this->writeOp(instruction->op(1), regvalue);
}

} // namespace REDasm
