#pragma once

#include "macros.h"
#include "types.h"
#include "plugin/loader.h"
#include "plugin/assembler.h"

DECLARE_HANDLE(RDDisassembler);

enum RDForkCondition {
    ForkCondition_None,
    ForkCondition_True,
    ForkCondition_False,
};

struct RDILCPU;

RD_API_EXPORT RDDisassembler* RDDisassembler_Create(const RDLoaderRequest* request, RDLoaderPlugin* ploader, RDAssemblerPlugin* passembler);
RD_API_EXPORT RDAssembler* RDDisassembler_GetAssembler(const RDDisassembler* d);
RD_API_EXPORT RDLoader* RDDisassembler_GetLoader(const RDDisassembler* d);
RD_API_EXPORT const char* RDDisassembler_GetAssemblerId(const RDDisassembler* d);
RD_API_EXPORT const char* RDDisassembler_GetLoaderId(const RDDisassembler* d);
RD_API_EXPORT RDDocument* RDDisassembler_GetDocument(const RDDisassembler* d);
RD_API_EXPORT RDBuffer* RDDisassembler_GetBuffer(const RDDisassembler* d);
RD_API_EXPORT const RDILCPU* RDDisassembler_GetILCPU(const RDDisassembler* d);
RD_API_EXPORT size_t RDDisassembler_Bits(const RDDisassembler* d);
RD_API_EXPORT size_t RDDisassembler_AddressWidth(const RDDisassembler* d);
RD_API_EXPORT size_t RDDisassembler_GetReferencesCount(const RDDisassembler* d, rd_address address);
RD_API_EXPORT size_t RDDisassembler_GetTargetsCount(const RDDisassembler* d, rd_address address);
RD_API_EXPORT size_t RDDisassembler_GetReferences(const RDDisassembler* d, rd_address address, const rd_address** references);
RD_API_EXPORT size_t RDDisassembler_GetTargets(const RDDisassembler* d, rd_address address, const rd_address** targets);
RD_API_EXPORT rd_type RDDisassembler_MarkPointer(RDDisassembler* d, rd_address address, rd_address fromaddress);
RD_API_EXPORT rd_type RDDisassembler_MarkLocation(RDDisassembler* d, rd_address address, rd_address fromaddress);
RD_API_EXPORT size_t RDDisassembler_MarkTable(RDDisassembler* d, rd_address startaddress, rd_address fromaddress, size_t count);
RD_API_EXPORT RDLocation RDDisassembler_GetTarget(const RDDisassembler* d, rd_address address);
RD_API_EXPORT RDLocation RDDisassembler_Dereference(const RDDisassembler* d, rd_address address);
RD_API_EXPORT const char* RDDisassembler_RegisterName(RDDisassembler* d, const RDInstruction* instruction, const RDOperand* op, rd_register_id r);
RD_API_EXPORT const char* RDDisassembler_FunctionHexDump(RDDisassembler* d, rd_address address, RDSymbol* symbol);
RD_API_EXPORT bool RDDisassembler_Decode(RDDisassembler* d, rd_address address, RDInstruction** instruction);
RD_API_EXPORT bool RDDisassembler_Encode(RDDisassembler* d, RDEncodedInstruction* encoded);
RD_API_EXPORT void RDDisassembler_PushReference(RDDisassembler* d, rd_address address, rd_address refby);
RD_API_EXPORT void RDDisassembler_PopReference(RDDisassembler* d, rd_address address, rd_address refby);
RD_API_EXPORT void RDDisassembler_CheckOperands(RDDisassembler* d, const RDInstruction* instruction);
RD_API_EXPORT void RDDisassembler_CheckOperand(RDDisassembler* d, const RDInstruction* instruction, const RDOperand* op);
RD_API_EXPORT void RDDisassembler_Enqueue(RDDisassembler* d, rd_address address);
RD_API_EXPORT void RDDisassembler_EnqueueAddress(RDDisassembler* d, rd_address address, const RDInstruction* instruction);
RD_API_EXPORT void RDDisassembler_Next(RDDisassembler* d, const RDInstruction* instruction);
RD_API_EXPORT void RDDisassembler_Continue(RDDisassembler* d, rd_address address, const RDInstruction* instruction);
RD_API_EXPORT void RDDisassembler_Branch(RDDisassembler* d, rd_address address, const RDInstruction* instruction);
RD_API_EXPORT void RDDisassembler_UnlinkNext(RDDisassembler* d, rd_address address);

// Extra Functions
RD_API_EXPORT const char* RD_HexDump(const RDDisassembler* d, rd_address address, size_t size);
RD_API_EXPORT const char* RD_ReadString(const RDDisassembler* d, rd_address address, size_t* len);
RD_API_EXPORT const char16_t* RD_ReadWString(const RDDisassembler* d, rd_address address, size_t* len);
RD_API_EXPORT void RD_DisassembleAddress(RDDisassembler* d, rd_address address);
RD_API_EXPORT void RD_Disassemble(RDDisassembler* d);

#ifdef __cplusplus
struct InstructionLock {
    InstructionLock(RDDisassembler* d, rd_address address): m_document(RDDisassembler_GetDocument(d)) { RDDisassembler_Decode(d, address, &m_instruction); }
    InstructionLock(RDDocument* d, rd_address address): m_document(d) { this->lock(address); }
    InstructionLock(RDDocument* d, const RDLocation& loc): m_document(d) { if(loc.valid) this->lock(loc.address); }
    ~InstructionLock() { this->unlock(); }
    void lock(rd_address address) { this->unlock(); RDDocument_LockInstruction(m_document, address, &m_instruction); }
    void unlock() { if(m_document && m_instruction) RDDocument_UnlockInstruction(m_document, m_instruction); }
    RDInstruction* operator *() const { return m_instruction; }
    RDInstruction* operator ->() const { return m_instruction; }
    operator bool() const { return m_instruction; }

    InstructionLock() = delete;
    InstructionLock(const InstructionLock&) = delete;
    void operator=(InstructionLock&) = delete;

    private:
        RDDocument* m_document{nullptr};
        RDInstruction* m_instruction{nullptr};
};
#endif
