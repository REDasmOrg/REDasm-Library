#include "loader.h"
#include <rdcore/context.h>
#include <rdcore/plugin/loader.h>

bool RDLoader_Register(RDLoaderPlugin* ploader) { return rd_ctx->registerPlugin(ploader); }
const char* RDLoader_GetAssemblerId(const RDLoaderPlugin* ploader) { return rd_ctx->getAssemblerId(ploader); }
RDAssemblerPlugin* RDLoader_GetAssemblerPlugin(const RDLoaderPlugin* ploader) { return rd_ctx->getAssembler(ploader); }
RDDocument* RDLoader_GetDocument(RDLoader* ldr) { return CPTR(RDDocument, std::addressof(CPTR(Loader, ldr)->document())); }
RDDatabase* RDLoader_GetDatabase(RDLoader* ldr, const char* dbname) { return CPTR(RDDatabase, CPTR(Loader, ldr)->database(dbname)); }
RDBuffer* RDLoader_GetBuffer(RDLoader* ldr) { return CPTR(RDBuffer, CPTR(Loader, ldr)->buffer()); }
rd_flag RDLoader_GetFlags(const RDLoader* ldr) { return CPTR(const Loader, ldr)->flags(); }
u8* RDLoader_GetData(RDLoader* ldr) { return CPTR(Loader, ldr)->buffer()->data(); }
bool RDLoader_Build(RDLoader* ldr, const RDLoaderBuildRequest* req) { return CPTR(Loader, ldr)->build(req); }
bool RDLoader_Load(RDLoader* ldr) { return CPTR(Loader, ldr)->load(); }

void RD_GetLoaders(const RDLoaderRequest* loadrequest, Callback_LoaderPlugin callback, void* userdata) { rd_ctx->getLoaders(loadrequest, callback, userdata); }
u8* RD_AddrPointer(const RDLoader* ldr, rd_address address) { return CPTR(const Loader, ldr)->addrpointer(address); }
u8* RD_Pointer(const RDLoader* ldr, rd_offset offset) { return CPTR(const Loader, ldr)->pointer(offset); }
RDLocation RD_FileOffset(const RDLoader* ldr, const void* ptr) { return CPTR(const Loader, ldr)->fileoffset(ptr); }
RDLocation RD_AddressOf(const RDLoader* ldr, const void* ptr) { return CPTR(const Loader, ldr)->addressof(ptr); }
RDLocation RD_Offset(const RDLoader* ldr, rd_address address) { return CPTR(const Loader, ldr)->offset(address); }
RDLocation RD_Address(const RDLoader* ldr, rd_offset offset) { return CPTR(const Loader, ldr)->address(offset); }
