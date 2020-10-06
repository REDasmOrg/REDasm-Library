#include "assembler.h"
#include <rdcore/plugin/assembler.h>
#include <rdcore/plugin/interface/pluginmodule.h>
#include <algorithm>

bool RDAssembler_Register(RDPluginModule* m, const RDEntryAssembler* entry) { return CPTR(PluginModule, m)->registerEntry(entry); }
const char* RDAssembler_GetId(const RDAssembler* assembler) { return CPTR(const Assembler, assembler)->id(); }
const char* RDAssembler_GetName(const RDAssembler* assembler) { return CPTR(const Assembler, assembler)->name(); }

void RDEncodedInstruction_Init(RDEncodedInstruction* encoded) { *encoded = { }; }

bool RDEncodedInstruction_Set(RDEncodedInstruction* encoded, u8* encdata, size_t count)
{
    if(count >= ENCODE_BUFFER_SIZE) return false;
    std::copy_n(encdata, count, encoded->encoded);
    encoded->count = count;
    return true;
}
