#include "loader.h"
#include <rdcore/plugin/interface/pluginmodule.h>
#include <rdcore/document/document.h>
#include <rdcore/plugin/loader.h>

bool RDLoader_Register(RDPluginModule* pm, const RDEntryLoader* entry) { return CPTR(PluginModule, pm)->registerEntry(entry); }
rd_flag RDLoader_GetFlags(const RDLoader* ldr) { return CPTR(const Loader, ldr)->flags(); }
const char* RDLoader_GetId(const RDLoader* ldr) { return CPTR(const Loader, ldr)->id(); }
const char* RDLoader_GetName(const RDLoader* ldr) { return CPTR(const Loader, ldr)->name(); }
rd_endianness RDLoader_GetEndianness(const RDLoader* ldr) { return CPTR(const Loader, ldr)->document()->endianness(); }
