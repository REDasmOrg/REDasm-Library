#include "plugin.h"
#include <rdcore/context.h>

void RDPlugin_Free(RDPluginHeader* plugin) { Context::freePlugin(plugin); }
