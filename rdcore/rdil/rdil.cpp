#include "rdil.h"
#include "../support/utils.h"
#include "../support/sugar.h"
#include "../disassembler.h"
#include "isa.h"

std::string RDIL::disasm(const Disassembler* disassembler, const RDInstruction* rdil, const RDInstruction* instruction)
{
    if(rdil->id >= RDILOpcodes.size()) return std::string();

    auto& opcode = RDILOpcodes[rdil->id];

    std::stringstream ss;
    ss << opcode.mnemonic << " ";

    for(size_t i = 0; i < opcode.operandscount; i++)
        RDIL::darg(ss, rdil->operands[i], instruction, disassembler, i);

    return ss.str();
}

void RDIL::emitRDIL(RDInstruction* rdil, rd_instruction_id id)
{
    auto& opcode = RDILOpcodes[id];

    rdil->operandscount = opcode.operandscount;
    rdil->id = id;
    rdil->type = opcode.type;
    rdil->flags = opcode.flags;

    for(size_t i = 0; i < opcode.operandscount; i++)
    {
        auto& optype = opcode.operands[i];
        rdil->operands[i].type = optype;

        if(optype == OperandType_Register)
            rdil->operands[i].reg = RD_NREG;
    }

    Sugar::setMnemonic(rdil, opcode.mnemonic);
}

void RDIL::darg(std::stringstream& ss, const RDOperand& op, const RDInstruction* instruction, const Disassembler* disassembler, size_t idx)
{
    if(IS_TYPE(&op, OperandType_Void)) return;
    if(idx) ss << ", ";

    switch(op.type)
    {
        case OperandType_Register:
            if(HAS_FLAG(&op, OperandFlags_Virtual)) {
                if(op.reg < RDILRegisters.size()) ss << RDILRegisters[op.reg];
                else ss << "$" << op.reg;
            }
            else ss << disassembler->registerName(instruction, op.reg);
            break;

        case OperandType_Constant:
        case OperandType_Immediate:
            ss << Utils::hex(op.u_value);
            break;

        case OperandType_Memory:
            ss << "[" <<  Utils::hex(op.u_value) << "]";
            break;

        case OperandType_Displacement:
            ss << "[" << disassembler->registerName(instruction, op.base);

            if(op.index != RD_NREG) {
                ss << "+" << disassembler->registerName(instruction, op.index);
                if(op.scale > 1) ss << "*" << op.scale;
            }

            if(op.displacement < 0)  ss << "-" << Utils::hex(op.displacement);
            else if(op.displacement > 0) ss << "+" << Utils::hex(op.displacement);
            ss << "]";
            break;

        default:
            ss << "???";
            break;
    }
}
