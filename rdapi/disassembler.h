#pragma once

#include "macros.h"
#include "types.h"
#include "net.h"
#include "plugin/loader.h"
#include "plugin/assembler/assembler.h"

DECLARE_HANDLE(RDDisassembler);

RD_API_EXPORT RDDisassembler* RDDisassembler_Create(const RDLoaderRequest* request, RDLoaderPlugin* ploader, RDAssemblerPlugin* passembler);
RD_API_EXPORT RDAssembler* RDDisassembler_GetAssembler(const RDDisassembler* d);
RD_API_EXPORT RDLoader* RDDisassembler_GetLoader(const RDDisassembler* d);
RD_API_EXPORT const RDNet* RDDisassembler_GetNet(const RDDisassembler* d);
RD_API_EXPORT const char* RDDisassembler_GetAssemblerId(const RDDisassembler* d);
RD_API_EXPORT const char* RDDisassembler_GetLoaderId(const RDDisassembler* d);
RD_API_EXPORT RDDocument* RDDisassembler_GetDocument(const RDDisassembler* d);
RD_API_EXPORT RDBuffer* RDDisassembler_GetBuffer(const RDDisassembler* d);
RD_API_EXPORT bool RDDisassembler_GetView(const RDDisassembler* d, rd_address address, size_t size, RDBufferView* view);
RD_API_EXPORT size_t RDDisassembler_Bits(const RDDisassembler* d);
RD_API_EXPORT size_t RDDisassembler_AddressWidth(const RDDisassembler* d);
RD_API_EXPORT size_t RDDisassembler_GetReferencesCount(const RDDisassembler* d, rd_address address);
RD_API_EXPORT RDLocation RDDisassembler_GetTarget(const RDDisassembler* d, rd_address address);
RD_API_EXPORT RDLocation RDDisassembler_Dereference(const RDDisassembler* d, rd_address address);
RD_API_EXPORT const char* RDDisassembler_FunctionHexDump(RDDisassembler* d, rd_address address, RDSymbol* symbol);
RD_API_EXPORT bool RDDisassembler_Encode(RDDisassembler* d, RDEncodedInstruction* encoded);

// Extra Functions
RD_API_EXPORT const char* RD_HexDump(const RDDisassembler* d, rd_address address, size_t size);
RD_API_EXPORT const char* RD_ReadString(const RDDisassembler* d, rd_address address, size_t* len);
RD_API_EXPORT const char16_t* RD_ReadWString(const RDDisassembler* d, rd_address address, size_t* len);
RD_API_EXPORT void RD_DisassembleAddress(RDDisassembler* d, rd_address address);
RD_API_EXPORT void RD_Disassemble(RDDisassembler* d);
