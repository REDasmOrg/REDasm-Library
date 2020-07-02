#include "rdil.h"
#include <rdcore/rdil/ilcpu.h>
#include <rdcore/disassembler.h>

void RDIL_Disassemble(RDDisassembler* d, rd_address startaddress, Callback_DisassembleRDIL cbrdil, void* userdata) { CPTR(Disassembler, d)->disassembleRDIL(startaddress, cbrdil, userdata); }

void RDIL_SetILRegister(RDInstruction* rdil, size_t idx, rd_register_id r)
{
    if(idx >= DEFAULT_CONTAINER_SIZE) return;

    rdil->operands[idx].type = OperandType_Register;
    rdil->operands[idx].flags = OperandFlags_Virtual;
    rdil->operands[idx].reg = r;
}

void RDIL_SetRegister(RDInstruction* rdil, size_t idx, rd_register_id r)
{
    if(idx >= DEFAULT_CONTAINER_SIZE) return;

    rdil->operands[idx].type = OperandType_Register;
    rdil->operands[idx].flags = OperandFlags_None;
    rdil->operands[idx].reg = r;
}

void RDIL_SetValue(RDInstruction* rdil, size_t idx, u64 v)
{
    if(idx >= DEFAULT_CONTAINER_SIZE) return;

    rdil->operands[idx].type = OperandType_Constant;
    rdil->operands[idx].flags = OperandFlags_None;
    rdil->operands[idx].u_value = v;
}

void RDIL_SetOperand(RDInstruction* rdil, size_t idx, const RDOperand* op)
{
    if(idx >= DEFAULT_CONTAINER_SIZE) return;

    rdil->operands[idx] = *op;
}

void RDIL_EmitUNKNOWN(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Unknown); }
void RDIL_EmitUNDEF(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Undef); }
void RDIL_EmitNOP(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Nop); }
void RDIL_EmitADD(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Add); }
void RDIL_EmitSUB(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Sub); }
void RDIL_EmitMUL(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Mul); }
void RDIL_EmitDIV(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Div); }
void RDIL_EmitMOD(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Mod); }
void RDIL_EmitAND(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_And); }
void RDIL_EmitOR(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Or); }
void RDIL_EmitXOR(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Xor); }
void RDIL_EmitNOT(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Not); }
void RDIL_EmitBSH(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Bsh); }
void Rdil_EmitBISZ(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Bisz); }
void RDIL_EmitJMP(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Jmp); }
void RDIL_EmitJMPZ(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Jz); }
void RDIL_EmitJMPNZ(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Jnz); }
void RDIL_EmitCALL(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Call); }
void RDIL_EmitCALLZ(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Cz); }
void RDIL_EmitCALLNZ(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Cnz); }
void RDIL_EmitRET(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Ret); }
void RDIL_EmitCOPY(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Copy); }
void RDIL_EmitLOAD(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Load); }
void RDIL_EmitSTORE(RDInstruction* rdil) { RDIL::emitRDIL(rdil, RDIL_Store); }
