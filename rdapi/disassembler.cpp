#include "disassembler.h"
#include <rdcore/disassembler.h>

RDDisassembler* RDDisassembler_Create(const RDLoaderRequest* request, RDLoaderPlugin* ploader, RDAssemblerPlugin* passembler)
{
    return CPTR(RDDisassembler, new Disassembler(request, ploader, passembler));
}

bool RD_DisassemblerBusy(const RDDisassembler* d) { return CPTR(const Disassembler, d)->busy(); }
RDAssemblerPlugin* RDDisassembler_GetAssembler(const RDDisassembler* d) { return CPTR(const Disassembler, d)->assembler(); }
RDLoader* RDDisassembler_GetLoader(const RDDisassembler* d) { return CPTR(RDLoader, CPTR(const Disassembler, d)->loader()); }
RDDocument* RDDisassembler_GetDocument(const RDDisassembler* d) { return CPTR(RDDocument, std::addressof(CPTR(const Disassembler, d)->document())); }
RDBuffer* RDDisassembler_GetBuffer(const RDDisassembler* d) { return CPTR(RDBuffer, CPTR(const Disassembler, d)->buffer()); }
size_t RDDisassembler_Bits(const RDDisassembler* d) { return CPTR(const Disassembler, d)->bits(); }
size_t RDDisassembler_AddressWidth(const RDDisassembler* d) { return CPTR(const Disassembler, d)->addressWidth(); }
size_t RDDisassembler_GetReferencesCount(const RDDisassembler* d, address_t address) { return CPTR(const Disassembler, d)->getReferencesCount(address); }
size_t RDDisassembler_GetTargetsCount(const RDDisassembler* d, address_t address) { return CPTR(const Disassembler, d)->getTargetsCount(address); }
size_t RDDisassembler_GetReferences(const RDDisassembler* d, address_t address, const address_t** references) { return CPTR(const Disassembler, d)->getReferences(address, references); }
size_t RDDisassembler_GetTargets(const RDDisassembler* d, address_t address, const address_t** targets) { return CPTR(const Disassembler, d)->getTargets(address, targets); }
RDLocation RDDisassembler_GetTarget(const RDDisassembler* d, address_t address) { return CPTR(const Disassembler, d)->getTarget(address); }
RDLocation RDDisassembler_Dereference(const RDDisassembler* d, address_t address) { return CPTR(const Disassembler, d)->dereference(address); }

const char* RD_HexDump(const RDDisassembler* d, address_t address, RDSymbol* symbol) { return CPTR(const Disassembler, d)->getHexDump(address, symbol); }
const char* RD_ReadString(const RDDisassembler* d, address_t address, size_t* len) { return CPTR(const Disassembler, d)->readString(address, len); }
const char16_t* RD_ReadWString(const RDDisassembler* d, address_t address, size_t* len) { return CPTR(const Disassembler, d)->readWString(address, len); }
void RD_DisassembleAddress(RDDisassembler* d, address_t address) { CPTR(Disassembler, d)->disassembleAddress(address); }
void RD_Disassemble(RDDisassembler* d) { CPTR(Disassembler, d)->disassemble(); }
