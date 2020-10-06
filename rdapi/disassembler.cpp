#include "disassembler.h"
#include <rdcore/disassembler.h>

const char* RDDisassembler_FunctionHexDump(RDDisassembler* d, rd_address address, RDSymbol* symbol) { return CPTR(const Disassembler, d)->getFunctionHexDump(address, symbol);  }
RDLocation RDDisassembler_Dereference(const RDDisassembler* d, rd_address address) { return CPTR(const Disassembler, d)->dereference(address); }
bool RDDisassembler_Encode(RDDisassembler* d, RDEncodedInstruction* encoded) { return CPTR(Disassembler, d)->encode(encoded); }
bool RDDisassembler_CreateFunction(RDDisassembler* d, rd_address address, const char* name) { return CPTR(Disassembler, d)->createFunction(address, name); }
bool RDDisassembler_ScheduleFunction(RDDisassembler* d, rd_address address, const char* name) { return CPTR(Disassembler, d)->scheduleFunction(address, name); }
void RDDisassembler_Enqueue(RDDisassembler* d, rd_address address) { CPTR(Disassembler, d)->enqueue(address); }
void RDDisassembler_Schedule(RDDisassembler* d, rd_address address) { CPTR(Disassembler, d)->schedule(address); }

const char* RD_HexDump(const RDDisassembler* d, rd_address address, size_t size) { return CPTR(const Disassembler, d)->getHexDump(address, size); }
const char* RD_ReadString(const RDDisassembler* d, rd_address address, size_t* len) { return CPTR(const Disassembler, d)->readString(address, len); }
const char16_t* RD_ReadWString(const RDDisassembler* d, rd_address address, size_t* len) { return CPTR(const Disassembler, d)->readWString(address, len); }
