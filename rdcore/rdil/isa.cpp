#include "isa.h"

const std::array<RDILOpcode, RDIL_Count> RDILOpcodes = {{
    // VM
    { "unknown", RDIL_Unknown, InstructionType_None, InstructionFlags_None, { 0, 0, 0 }, 0 },
    { "undef", RDIL_Undef, InstructionType_None, InstructionFlags_None, { OperandType_Register, 0, 0 }, 1 },

    // Nop
    { "nop", RDIL_Nop, InstructionType_Nop, InstructionFlags_None, { 0, 0, 0 }, 0 },

    // Arithmetic
    { "add", RDIL_Add, InstructionType_Add, InstructionFlags_None, { 0, 0, 0 }, 3 },
    { "sub", RDIL_Sub, InstructionType_Sub, InstructionFlags_None, { 0, 0, 0 }, 3 },
    { "mul", RDIL_Mul, InstructionType_Mul, InstructionFlags_None, { 0, 0, 0 }, 3 },
    { "div", RDIL_Div, InstructionType_Div, InstructionFlags_None, { 0, 0, 0 }, 3 },
    { "mod", RDIL_Mod, InstructionType_Mod, InstructionFlags_None, { 0, 0, 0 }, 3 },

    // Logical
    { "and", RDIL_And, InstructionType_And, InstructionFlags_None, { 0, 0, 0 }, 3 },
    { "or",  RDIL_Or,  InstructionType_Or,  InstructionFlags_None, { 0, 0, 0 }, 3 },
    { "xor", RDIL_Xor, InstructionType_Xor, InstructionFlags_None, { 0, 0, 0 }, 3 },
    { "not", RDIL_Not, InstructionType_Not, InstructionFlags_None, { 0, 0, 0 }, 3 },
    { "bsh", RDIL_Bsh, InstructionType_None, InstructionFlags_None, { 0, 0, 0 }, 3 },

    // Compare
    { "bisz",  RDIL_Bisz, InstructionType_Compare, InstructionFlags_None, { 0, 0, 0 }, 2 },

    // Branch
    { "jmp",  RDIL_Jmp,  InstructionType_Jump, InstructionFlags_None, { 0, 0, 0 }, 1 },
    { "jz",   RDIL_Jz,   InstructionType_Jump, InstructionFlags_Conditional, { 0, 0, 0 }, 1 },
    { "jnz",  RDIL_Jnz,  InstructionType_Jump, InstructionFlags_Conditional, { 0, 0, 0 }, 1 },
    { "call", RDIL_Call, InstructionType_Call, InstructionFlags_None, { 0, 0, 0 }, 1 },
    { "cz",   RDIL_Cz,   InstructionType_Call, InstructionFlags_Conditional, { 0, 0, 0 }, 1 },
    { "cnz",  RDIL_Cnz,  InstructionType_Call, InstructionFlags_Conditional, { 0, 0, 0 }, 1 },
    { "ret",  RDIL_Ret,  InstructionType_Ret, InstructionFlags_Stop, { 0, 0, 0 }, 1 },

    // Read/Write
    { "copy",  RDIL_Copy,  InstructionType_None,  InstructionFlags_None, { 0, 0, 0 }, 2 },
    { "load",  RDIL_Load,  InstructionType_Load,  InstructionFlags_None, { 0, OperandType_Displacement, 0 }, 2 },
    { "store", RDIL_Store, InstructionType_Store, InstructionFlags_None, { 0, OperandType_Displacement, 0 }, 2 }
}};


extern const std::array<const char*, RDILRegister_Count> RDILRegisters = {{
    "$zero", "$cond", "$ret"
}};
