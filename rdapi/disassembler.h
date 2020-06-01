#pragma once

#include "macros.h"
#include "types.h"
#include "plugin/loader.h"
#include "plugin/assembler.h"

DECLARE_HANDLE(RDDisassembler);

RD_API_EXPORT RDDisassembler* RDDisassembler_Create(const RDLoaderRequest* request, RDLoaderPlugin* ploader, RDAssemblerPlugin* passembler);
RD_API_EXPORT RDAssemblerPlugin* RDDisassembler_GetAssembler(const RDDisassembler* d);
RD_API_EXPORT RDLoader* RDDisassembler_GetLoader(const RDDisassembler* d);
RD_API_EXPORT RDDocument* RDDisassembler_GetDocument(const RDDisassembler* d);
RD_API_EXPORT RDBuffer* RDDisassembler_GetBuffer(const RDDisassembler* d);
RD_API_EXPORT const RDBlockContainer* RDDisassembler_GetBlocks(const RDDisassembler* d);
RD_API_EXPORT size_t RDDisassembler_Bits(const RDDisassembler* d);
RD_API_EXPORT size_t RDDisassembler_AddressWidth(const RDDisassembler* d);
RD_API_EXPORT size_t RDDisassembler_GetReferencesCount(const RDDisassembler* d, address_t address);
RD_API_EXPORT size_t RDDisassembler_GetTargetsCount(const RDDisassembler* d, address_t address);
RD_API_EXPORT size_t RDDisassembler_GetReferences(const RDDisassembler* d, address_t address, const address_t** references);
RD_API_EXPORT size_t RDDisassembler_GetTargets(const RDDisassembler* d, address_t address, const address_t** targets);
RD_API_EXPORT type_t RDDisassembler_MarkPointer(RDDisassembler* d, address_t address, address_t fromaddress);
RD_API_EXPORT type_t RDDisassembler_MarkLocation(RDDisassembler* d, address_t address, address_t fromaddress);
RD_API_EXPORT size_t RDDisassembler_MarkTable(RDDisassembler* d, address_t startaddress, address_t fromaddress, size_t count);
RD_API_EXPORT RDLocation RDDisassembler_GetTarget(const RDDisassembler* d, address_t address);
RD_API_EXPORT RDLocation RDDisassembler_Dereference(const RDDisassembler* d, address_t address);
RD_API_EXPORT const char* RDDisassembler_RegisterName(RDDisassembler* d, const RDInstruction* instruction, register_id_t r);
RD_API_EXPORT const char* RDDisassembler_FunctionHexDump(RDDisassembler* d, address_t address, RDSymbol* symbol);
RD_API_EXPORT bool RDDisassembler_Decode(RDDisassembler* d, address_t address, RDInstruction** instruction);
RD_API_EXPORT bool RDDisassembler_Encode(RDDisassembler* d, RDEncodedInstruction* encoded);
RD_API_EXPORT void RDDisassembler_PushReference(RDDisassembler* d, address_t address, address_t refby);
RD_API_EXPORT void RDDisassembler_PopReference(RDDisassembler* d, address_t address, address_t refby);
RD_API_EXPORT void RDDisassembler_HandleOperand(RDDisassembler* d, const RDInstruction* instruction, const RDOperand* op);
RD_API_EXPORT void RDDisassembler_Enqueue(RDDisassembler* d, address_t address);
RD_API_EXPORT void RDDisassembler_EnqueueAddress(RDDisassembler* d, const RDInstruction* instruction, address_t address);
RD_API_EXPORT void RDDisassembler_EnqueueNext(RDDisassembler* d, const RDInstruction* instruction);

// Extra Functions
RD_API_EXPORT const char* RD_HexDump(const RDDisassembler* d, address_t address, size_t size);
RD_API_EXPORT const char* RD_ReadString(const RDDisassembler* d, address_t address, size_t* len);
RD_API_EXPORT const char16_t* RD_ReadWString(const RDDisassembler* d, address_t address, size_t* len);
RD_API_EXPORT void RD_DisassembleAddress(RDDisassembler* d, address_t address);
RD_API_EXPORT void RD_Disassemble(RDDisassembler* d);

#ifdef __cplusplus
struct InstructionLock {
    InstructionLock(RDDisassembler* d, address_t address): m_document(RDDisassembler_GetDocument(d)) { RDDisassembler_Decode(d, address, &m_instruction); }
    InstructionLock(RDDocument* d, address_t address): m_document(d) { this->lock(address); }
    InstructionLock(RDDocument* d, const RDLocation& loc): m_document(d) { if(loc.valid) this->lock(loc.address); }
    ~InstructionLock() { this->unlock(); }
    void lock(address_t address) { this->unlock(); RDDocument_LockInstruction(m_document, address, &m_instruction); }
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
