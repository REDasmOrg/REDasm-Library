#include "assembler.h"
#include <rdcore/context.h>

bool RDAssembler_Register(RDAssemblerPlugin* plugin) { return rd_ctx->registerPlugin(plugin); }
void RD_GetAssemblers(Callback_AssemblerPlugin callback, void* userdata) { return rd_ctx->getAssemblers(callback, userdata); }
RDAssemblerPlugin* RDAssembler_Find(const char* id) { return rd_ctx->findAssembler(id); }
