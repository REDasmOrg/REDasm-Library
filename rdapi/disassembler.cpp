#include "disassembler.h"
#include <rdcore/disassembler.h>

RDDisassembler* RDDisassembler_Create(const RDLoaderRequest* request, RDLoaderPlugin* ploader, RDAssemblerPlugin* passembler) { return CPTR(RDDisassembler, new Disassembler(request, ploader, passembler)); }
RDAssembler* RDDisassembler_GetAssembler(const RDDisassembler* d) { return CPTR(RDAssembler, CPTR(const Disassembler, d)->assembler()); }
RDLoader* RDDisassembler_GetLoader(const RDDisassembler* d) { return CPTR(RDLoader, CPTR(const Disassembler, d)->loader()); }
bool RDDisassembler_GetAssemblerUserData(const RDDisassembler* d, RDUserData* userdata) { return CPTR(const Disassembler, d)->loader()->getUserData(userdata); }
bool RDDisassembler_GetLoaderUserData(const RDDisassembler* d, RDUserData* userdata) { return CPTR(const Disassembler, d)->assembler()->getUserData(userdata); }
const RDNet* RDDisassembler_GetNet(const RDDisassembler* d) { return CPTR(const RDNet, CPTR(const Disassembler, d)->net()); }
const char* RDDisassembler_FunctionHexDump(RDDisassembler* d, rd_address address, RDSymbol* symbol) { return CPTR(const Disassembler, d)->getFunctionHexDump(address, symbol);  }
const char* RDDisassembler_GetAssemblerId(const RDDisassembler* d) { return CPTR(const Disassembler, d)->assembler()->id(); }
const char* RDDisassembler_GetLoaderId(const RDDisassembler* d) { return CPTR(const Disassembler, d)->loader()->id(); }
RDDocument* RDDisassembler_GetDocument(const RDDisassembler* d) { return CPTR(RDDocument, std::addressof(CPTR(const Disassembler, d)->document())); }
RDBuffer* RDDisassembler_GetBuffer(const RDDisassembler* d) { return CPTR(RDBuffer, CPTR(const Disassembler, d)->buffer()); }
bool RDDisassembler_GetView(const RDDisassembler* d, rd_address address, size_t size, RDBufferView* view) { return CPTR(const Disassembler, d)->view(address, size, view); }
size_t RDDisassembler_Bits(const RDDisassembler* d) { return CPTR(const Disassembler, d)->assembler()->bits(); }
size_t RDDisassembler_AddressWidth(const RDDisassembler* d) { return CPTR(const Disassembler, d)->assembler()->addressWidth(); }
RDLocation RDDisassembler_Dereference(const RDDisassembler* d, rd_address address) { return CPTR(const Disassembler, d)->dereference(address); }
bool RDDisassembler_Encode(RDDisassembler* d, RDEncodedInstruction* encoded) { return CPTR(Disassembler, d)->encode(encoded); }
void RDDisassembler_Enqueue(RDDisassembler* d, rd_address address) { CPTR(Disassembler, d)->enqueue(address); }
void RDDisassembler_Schedule(RDDisassembler* d, rd_address address) { CPTR(Disassembler, d)->schedule(address); }

const char* RD_HexDump(const RDDisassembler* d, rd_address address, size_t size) { return CPTR(const Disassembler, d)->getHexDump(address, size); }
const char* RD_ReadString(const RDDisassembler* d, rd_address address, size_t* len) { return CPTR(const Disassembler, d)->readString(address, len); }
const char16_t* RD_ReadWString(const RDDisassembler* d, rd_address address, size_t* len) { return CPTR(const Disassembler, d)->readWString(address, len); }
void RD_DisassembleAt(RDDisassembler* d, rd_address address) { CPTR(Disassembler, d)->disassembleAt(address); }
void RD_Disassemble(RDDisassembler* d) { CPTR(Disassembler, d)->disassemble(); }
