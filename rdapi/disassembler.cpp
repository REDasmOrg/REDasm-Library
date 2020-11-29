#include "disassembler.h"
#include <rdcore/disassembler.h>

void RDDisassembler_Enqueue(RDDisassembler* d, rd_address address) { CPTR(Disassembler, d)->enqueue(address); }

const char* RD_HexDump(const RDDisassembler* d, rd_address address, size_t size) { return CPTR(const Disassembler, d)->getHexDump(address, size); }
const char* RD_ReadString(const RDDisassembler* d, rd_address address, size_t* len) { return CPTR(const Disassembler, d)->readString(address, len); }
const char16_t* RD_ReadWString(const RDDisassembler* d, rd_address address, size_t* len) { return CPTR(const Disassembler, d)->readWString(address, len); }
