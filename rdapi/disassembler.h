#pragma once

#include "macros.h"
#include "types.h"
#include "loader.h"
#include "assembler.h"

typedef u32 state_t;

typedef struct RDState {
    const char* name;
    state_t id;

    union {
        u64 u_value;
        s64 s_value;
        address_t address;
    };

    size_t opindex;
    const RDInstruction* instruction;
} RDState;

DECLARE_HANDLE(RDDisassembler);

RD_API_EXPORT RDDisassembler* RDDisassembler_Create(const RDLoaderRequest* request, RDLoaderPlugin* ploader, RDAssemblerPlugin* passembler);
RD_API_EXPORT RDAssemblerPlugin* RDDisassembler_GetAssembler(const RDDisassembler* d);
RD_API_EXPORT RDLoader* RDDisassembler_GetLoader(const RDDisassembler* d);
RD_API_EXPORT RDDocument* RDDisassembler_GetDocument(const RDDisassembler* d);
RD_API_EXPORT RDBuffer* RDDisassembler_GetBuffer(const RDDisassembler* d);
RD_API_EXPORT size_t RDDisassembler_Bits(const RDDisassembler* d);
RD_API_EXPORT size_t RDDisassembler_AddressWidth(const RDDisassembler* d);
RD_API_EXPORT size_t RDDisassembler_GetReferencesCount(const RDDisassembler* d, address_t address);
RD_API_EXPORT size_t RDDisassembler_GetTargetsCount(const RDDisassembler* d, address_t address);
RD_API_EXPORT size_t RDDisassembler_GetReferences(const RDDisassembler* d, address_t address, const address_t** references);
RD_API_EXPORT size_t RDDisassembler_GetTargets(const RDDisassembler* d, address_t address, const address_t** targets);
RD_API_EXPORT RDLocation RDDisassembler_GetTarget(const RDDisassembler* d, address_t address);
RD_API_EXPORT RDLocation RDDisassembler_Dereference(const RDDisassembler* d, address_t address);

// Extra Functions
RD_API_EXPORT const char* RD_HexDump(const RDDisassembler* d, address_t address, RDSymbol* symbol);
RD_API_EXPORT const char* RD_ReadString(const RDDisassembler* d, address_t address, size_t* len);
RD_API_EXPORT const char16_t* RD_ReadWString(const RDDisassembler* d, address_t address, size_t* len);
RD_API_EXPORT void RD_DisassembleAddress(RDDisassembler* d, address_t address);
RD_API_EXPORT void RD_Disassemble(RDDisassembler* d);
