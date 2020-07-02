#pragma once

#include <rdapi/rdapi.h>
#include <array>

#define RDIL_MAX_OPERANDS 3

struct RDILOpcode
{
    const char* mnemonic;
    rd_instruction_id id;
    rd_type type;
    rd_flag flags;
    rd_type operands[RDIL_MAX_OPERANDS];
    size_t operandscount;
};

extern const std::array<RDILOpcode, RDIL_Count> RDILOpcodes;
extern const std::array<const char*, RDILRegister_Count> RDILRegisters;
