#include "disassembler.h"
#include <rdcore/disassembler.h>

RDLocation RDDisassembler_Dereference(const RDDisassembler* d, rd_address address) { return CPTR(const Disassembler, d)->dereference(address); }
bool RDDisassembler_Encode(RDDisassembler* d, RDEncodedInstruction* encoded) { return CPTR(Disassembler, d)->encode(encoded); }
void RDDisassembler_Enqueue(RDDisassembler* d, rd_address address) { CPTR(Disassembler, d)->enqueue(address); }
void RDDisassembler_Schedule(RDDisassembler* d, rd_address address) { CPTR(Disassembler, d)->schedule(address); }

const char* RD_HexDump(const RDDisassembler* d, rd_address address, size_t size) { return CPTR(const Disassembler, d)->getHexDump(address, size); }
const char* RD_ReadString(const RDDisassembler* d, rd_address address, size_t* len) { return CPTR(const Disassembler, d)->readString(address, len); }
const char16_t* RD_ReadWString(const RDDisassembler* d, rd_address address, size_t* len) { return CPTR(const Disassembler, d)->readWString(address, len); }
