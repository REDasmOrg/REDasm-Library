#pragma once

// Based on: https://www.zynamics.com/binnavi/manual/html/reil_language.htm

#include "macros.h"
#include "types.h"

#define RDIL_INSTRUCTION_COUNT 8

enum RDILRegisters {
    RDILRegister_Zero = 0, // "Zero" Register
    RDILRegister_Cond,     // "Conditional" Register
    RDILRegister_Ret,      // "Return" Register

    RDILRegister_Count,
    RDILRegister_Base = 0x1000, // General Registers
};

#define RDIL_REGISTER(n) (RDILRegister_Base + n)
#define RDIL_ADVANCE(rdil) (*rdil)++

enum RDILOpcodes {
    // VM
    RDIL_Unknown = 0, //                      unknown
    RDIL_Undef,       //                      undef src1

    // Nop
    RDIL_Nop,         //                      nop

    // Arithmetic
    RDIL_Add,         // dst = src1 + src2    add dst, src1, src2
    RDIL_Sub,         // dst = src1 - src2    sub dst, src1, src2
    RDIL_Mul,         // dst = src1 * src2    mul dst, src1, src2
    RDIL_Div,         // dst = src1 / src2    div dst, src1, src2
    RDIL_Mod,         // dst = src1 % src2    mod dst, src1, src2

    // Logical
    RDIL_And,         // dst = src1 & src2    and dst, src1, src2
    RDIL_Or,          // dst = src1 | src2    or  dst, src1, src2
    RDIL_Xor,         // dst = src1 ^ src2    xor dst, src1, src2
    RDIL_Not,         // dst = ~src1          not dst, src1

    RDIL_Bsh,         // dst = src1 << src2   bsh dst, src1, src2 (src2 > 0)
                      // dst = src1 >> src2   bsh dst, src1, src2 (src2 < 0)

    // Compare
    RDIL_Bisz,        // dst = src1 == 0      bisz dst, src1

    // Branch
    RDIL_Jmp,         // Jump always          jmp src1
    RDIL_Jz,          // Jump if $rc == 0     jmpz src1
    RDIL_Jnz,         // Jump if $rc != 0     jmpnz src1
    RDIL_Call,        // Call always          call src1
    RDIL_Cz,          // Call if $rc == 0     cz src1
    RDIL_Cnz,         // Call if $rc != 0     cnz src1
    RDIL_Ret,         // Return from call     ret src1

    // Read/Write
    RDIL_Copy,        // dst = src            copy dst, src
    RDIL_Load,        // dst = src1[disp]     load dst, [src1 + disp]
    RDIL_Store,       // dst[disp] = src1     store src1, [dst + disp]

    // Meta
    RDIL_Count,
};

DECLARE_HANDLE(RDILCPU);

typedef struct RDILDisassembled {
    char result[DEFAULT_FULL_NAME_SIZE];
    RDInstruction rdil;
    rd_address address;
    size_t index;
} RDILDisassembled;

typedef void (*Callback_DisassembleRDIL)(const RDILDisassembled* d, void* userdata);

struct RDDisassembler;

RD_API_EXPORT bool RDILCPU_Read(const RDILCPU* cpu, const RDOperand* op, u64* val);

// Extra RDIL Functions
RD_API_EXPORT void RDIL_Disassemble(RDDisassembler* d, rd_address startaddress, Callback_DisassembleRDIL cbrdil, void* userdata);

RD_API_EXPORT void RDIL_SetOperand(RDInstruction* rdil, size_t idx, const RDOperand* op);
RD_API_EXPORT void RDIL_SetILRegister(RDInstruction* rdil, size_t idx, rd_register_id r);
RD_API_EXPORT void RDIL_SetRegister(RDInstruction* rdil, size_t idx, rd_register_id r);
RD_API_EXPORT void RDIL_SetValue(RDInstruction* rdil, size_t idx, u64 v);

RD_API_EXPORT void RDIL_EmitUNKNOWN(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitUNDEF(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitNOP(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitADD(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitSUB(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitMUL(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitDIV(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitMOD(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitAND(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitOR(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitXOR(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitNOT(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitBSH(RDInstruction* rdil);
RD_API_EXPORT void Rdil_EmitBISZ(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitJMP(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitJMPZ(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitJMPNZ(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitCALL(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitCALLZ(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitCALLNZ(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitRET(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitCOPY(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitLOAD(RDInstruction* rdil);
RD_API_EXPORT void RDIL_EmitSTORE(RDInstruction* rdil);
