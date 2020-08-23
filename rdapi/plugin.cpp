#include "plugin.h"
#include <rdcore/context.h>

void RDPlugin_Free(RDPluginHeader* plugin) { Context::freePlugin(plugin); }

bool RDPlugin_GetUserData(const RDPluginHeader* ldr, RDUserData* userdata)
{
    if(!userdata) return false;
    userdata->userdata = ldr->userdata;
    return true;
}
