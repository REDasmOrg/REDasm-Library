#include "disassembler.h"
#include <rdcore/disassembler.h>
#include <rdcore/support/sugar.h>

RDDisassembler* RDDisassembler_Create(const RDLoaderRequest* request, RDLoaderPlugin* ploader, RDAssemblerPlugin* passembler) { return CPTR(RDDisassembler, new Disassembler(request, ploader, passembler)); }
bool RD_DisassemblerBusy(const RDDisassembler* d) { return CPTR(const Disassembler, d)->busy(); }
RDAssemblerPlugin* RDDisassembler_GetAssembler(const RDDisassembler* d) { return CPTR(const Disassembler, d)->assembler(); }
RDLoader* RDDisassembler_GetLoader(const RDDisassembler* d) { return CPTR(RDLoader, CPTR(const Disassembler, d)->loader()); }
RDDocument* RDDisassembler_GetDocument(const RDDisassembler* d) { return CPTR(RDDocument, std::addressof(CPTR(const Disassembler, d)->document())); }
RDBuffer* RDDisassembler_GetBuffer(const RDDisassembler* d) { return CPTR(RDBuffer, CPTR(const Disassembler, d)->buffer()); }
const RDBlockContainer* RDDisassembler_GetBlocks(const RDDisassembler* d) { return CPTR(const RDBlockContainer, CPTR(const Disassembler, d)->document()->blocks()); }
size_t RDDisassembler_Bits(const RDDisassembler* d) { return CPTR(const Disassembler, d)->bits(); }
size_t RDDisassembler_AddressWidth(const RDDisassembler* d) { return CPTR(const Disassembler, d)->addressWidth(); }
size_t RDDisassembler_GetReferencesCount(const RDDisassembler* d, rd_address address) { return CPTR(const Disassembler, d)->getReferencesCount(address); }
size_t RDDisassembler_GetTargetsCount(const RDDisassembler* d, rd_address address) { return CPTR(const Disassembler, d)->getTargetsCount(address); }
size_t RDDisassembler_GetReferences(const RDDisassembler* d, rd_address address, const rd_address** references) { return CPTR(const Disassembler, d)->getReferences(address, references); }
size_t RDDisassembler_GetTargets(const RDDisassembler* d, rd_address address, const rd_address** targets) { return CPTR(const Disassembler, d)->getTargets(address, targets); }
rd_type RDDisassembler_MarkPointer(RDDisassembler* d, rd_address address, rd_address fromaddress) { return CPTR(Disassembler, d)->markPointer(address, fromaddress); }
rd_type RDDisassembler_MarkLocation(RDDisassembler* d, rd_address address, rd_address fromaddress) { return CPTR(Disassembler, d)->markLocation(address, fromaddress); }
size_t RDDisassembler_MarkTable(RDDisassembler* d, rd_address startaddress, rd_address fromaddress, size_t count) { return CPTR(Disassembler, d)->markTable(startaddress, fromaddress, count); }
RDLocation RDDisassembler_GetTarget(const RDDisassembler* d, rd_address address) { return CPTR(const Disassembler, d)->getTarget(address); }
RDLocation RDDisassembler_Dereference(const RDDisassembler* d, rd_address address) { return CPTR(const Disassembler, d)->dereference(address); }
bool RDDisassembler_Decode(RDDisassembler* d, rd_address address, RDInstruction** instruction) { return CPTR(Disassembler, d)->decode(address, instruction); }
bool RDDisassembler_Encode(RDDisassembler* d, RDEncodedInstruction* encoded) { return CPTR(Disassembler, d)->encode(encoded); }
void RDDisassembler_PushReference(RDDisassembler* d, rd_address address, rd_address refby) { CPTR(Disassembler, d)->pushReference(address, refby); }
void RDDisassembler_PopReference(RDDisassembler* d, rd_address address, rd_address refby) { CPTR(Disassembler, d)->popReference(address, refby); }
void RDDisassembler_CheckOperands(RDDisassembler* d, const RDInstruction* instruction) { CPTR(Disassembler, d)->checkOperands(instruction);  }
void RDDisassembler_CheckOperand(RDDisassembler* d, const RDInstruction* instruction, const RDOperand* op) { CPTR(Disassembler, d)->checkOperand(instruction, op); }
void RDDisassembler_Enqueue(RDDisassembler* d, rd_address address) { CPTR(Disassembler, d)->enqueue(address); }
void RDDisassembler_EnqueueAddress(RDDisassembler* d, const RDInstruction* instruction, rd_address address) { CPTR(Disassembler, d)->enqueueAddress(instruction, address); }
void RDDisassembler_EnqueueNext(RDDisassembler* d, const RDInstruction* instruction) { CPTR(Disassembler, d)->enqueue(Sugar::nextAddress(instruction)); }
const char* RDDisassembler_FunctionHexDump(RDDisassembler* d, rd_address address, RDSymbol* symbol) { return CPTR(const Disassembler, d)->getFunctionHexDump(address, symbol);  }

const char* RDDisassembler_RegisterName(RDDisassembler* d, const RDInstruction* instruction, const RDOperand* op, rd_register_id r)
{
    static std::string s;
    s = CPTR(Disassembler, d)->registerName(instruction, op, r);
    return s.c_str();
}

const char* RD_HexDump(const RDDisassembler* d, rd_address address, size_t size) { return CPTR(const Disassembler, d)->getHexDump(address, size); }
const char* RD_ReadString(const RDDisassembler* d, rd_address address, size_t* len) { return CPTR(const Disassembler, d)->readString(address, len); }
const char16_t* RD_ReadWString(const RDDisassembler* d, rd_address address, size_t* len) { return CPTR(const Disassembler, d)->readWString(address, len); }
void RD_DisassembleAddress(RDDisassembler* d, rd_address address) { CPTR(Disassembler, d)->disassembleAddress(address); }
void RD_Disassemble(RDDisassembler* d) { CPTR(Disassembler, d)->disassemble(); }
