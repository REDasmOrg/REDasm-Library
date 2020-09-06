#include "assembler.h"
#include <rdcore/plugin/assembler.h>
#include <rdcore/context.h>
#include <algorithm>

const char* RDAssembler_GetId(const RDAssembler* assembler) { return CPTR(const Assembler, assembler)->id(); }

bool RDAssembler_GetUserData(const RDAssembler* assembler, RDUserData* userdata)
{
    if(!userdata) return false;
    userdata->userdata = CPTR(const Assembler, assembler)->plugin()->userdata;
    return true;
}

bool RDAssembler_Register(RDAssemblerPlugin* plugin) { return rd_ctx->registerPlugin(plugin); }
void RD_GetAssemblers(Callback_AssemblerPlugin callback, void* userdata) { return rd_ctx->getAssemblers(callback, userdata); }
RDAssemblerPlugin* RDAssembler_Find(const char* id) { return rd_ctx->findAssembler(id); }

void RDEncodedInstruction_Init(RDEncodedInstruction* encoded) { *encoded = { }; }

bool RDEncodedInstruction_Set(RDEncodedInstruction* encoded, u8* encdata, size_t count)
{
    if(count >= ENCODE_BUFFER_SIZE) return false;
    std::copy_n(encdata, count, encoded->encoded);
    encoded->count = count;
    return true;
}
