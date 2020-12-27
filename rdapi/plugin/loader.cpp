#include "loader.h"
#include <rdcore/plugin/interface/pluginmodule.h>
#include <rdcore/document/document.h>
#include <rdcore/plugin/loader.h>

bool RDLoader_Register(RDPluginModule* pm, const RDEntryLoader* entry) { return CPTR(PluginModule, pm)->registerEntry(entry); }
RDDocument* RDLoader_GetDocument(RDLoader* ldr) { return CPTR(RDDocument, std::addressof(CPTR(Loader, ldr)->document())); }
RDBuffer* RDLoader_GetBuffer(RDLoader* ldr) { return CPTR(RDBuffer, CPTR(Loader, ldr)->buffer()); }
rd_flag RDLoader_GetFlags(const RDLoader* ldr) { return CPTR(const Loader, ldr)->flags(); }
u8* RDLoader_GetData(RDLoader* ldr) { return CPTR(Loader, ldr)->buffer()->data(); }
const char* RDLoader_GetId(const RDLoader* ldr) { return CPTR(const Loader, ldr)->id(); }
const char* RDLoader_GetName(const RDLoader* ldr) { return CPTR(const Loader, ldr)->name(); }

u8* RD_AddrPointer(const RDLoader* ldr, rd_address address) { return CPTR(const Loader, ldr)->document()->addrpointer(address); }
u8* RD_Pointer(const RDLoader* ldr, rd_offset offset) { return CPTR(const Loader, ldr)->document()->offspointer(offset); }
RDLocation RD_FileOffset(const RDLoader* ldr, const void* ptr) { return CPTR(const Loader, ldr)->document()->fileoffset(ptr); }
RDLocation RD_AddressOf(const RDLoader* ldr, const void* ptr) { return CPTR(const Loader, ldr)->document()->addressof(ptr); }
RDLocation RD_Offset(const RDLoader* ldr, rd_address address) { return CPTR(const Loader, ldr)->document()->offset(address); }
RDLocation RD_Address(const RDLoader* ldr, rd_offset offset) { return CPTR(const Loader, ldr)->document()->address(offset); }
bool RD_IsAddress(const RDLoader* ldr, rd_address address) { return CPTR(const Loader, ldr)->document()->isAddress(address); }
rd_endianness RDLoader_GetEndianness(const RDLoader* ldr) { return CPTR(const Loader, ldr)->document()->endianness(); }
void RDLoader_SetEndianness(RDLoader* ldr, rd_endianness e) { CPTR(Loader, ldr)->document()->setEndianness(e); }
