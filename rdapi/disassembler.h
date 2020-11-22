#pragma once

#include "macros.h"
#include "types.h"
#include "symbol.h"
#include "plugin/assembler/assembler.h"

RD_HANDLE(RDDisassembler);

RD_API_EXPORT RDLocation RDDisassembler_Dereference(const RDDisassembler* d, rd_address address);
RD_API_EXPORT bool RDDisassembler_Encode(RDDisassembler* d, RDEncodedInstruction* encoded);
RD_API_EXPORT void RDDisassembler_Enqueue(RDDisassembler* d, rd_address address);
RD_API_EXPORT void RDDisassembler_Schedule(RDDisassembler* d, rd_address address);

// Extra Functions
RD_API_EXPORT const char* RD_HexDump(const RDDisassembler* d, rd_address address, size_t size);
RD_API_EXPORT const char* RD_ReadString(const RDDisassembler* d, rd_address address, size_t* len);
RD_API_EXPORT const char16_t* RD_ReadWString(const RDDisassembler* d, rd_address address, size_t* len);
