#pragma once

#include "../database/database.h"
#include "../document/document.h"
#include "../buffer.h"
#include "assembler/assembler.h"
#include "entry.h"

enum RDLoaderFlags {
    LoaderFlags_None               = 0,
    LoaderFlags_CustomAssembler    = (1 << 0),
    LoaderFlags_CustomAddressing   = (1 << 1),
};

typedef struct RDLoaderBuildParams {
    rd_offset offset;
    rd_address baseaddress;
    rd_address entrypoint;
} RDLoaderBuildParams;

typedef struct RDLoaderRequest {
    const char* filepath;
    RDBuffer* buffer;
    RDLoaderBuildParams buildparams;
} RDLoaderRequest;

DECLARE_HANDLE(RDLoader);

struct RDEntryLoader;
struct RDContext;

typedef const char* (*Callback_LoaderTest)(const RDLoaderRequest* request);
typedef bool (*Callback_LoaderLoad)(struct RDContext* ctx, RDLoader* loader);
typedef bool (*Callback_LoaderBuild)(struct RDContext* ctx, RDLoader* loader, const RDLoaderBuildParams* buildparams);

typedef struct RDEntryLoader {
    RD_ENTRY_HEADER
    rd_flag flags;

    Callback_LoaderTest test;
    Callback_LoaderLoad load;
    Callback_LoaderBuild build;
} RDEntryLoader;

RD_API_EXPORT bool RDLoader_Register(RDPluginModule* pm, const RDEntryLoader* entry);
RD_API_EXPORT const char* RDLoader_GetId(const RDLoader* ldr);
RD_API_EXPORT const char* RDLoader_GetName(const RDLoader* ldr);
RD_API_EXPORT RDEntryAssembler* RDLoader_GetAssemblerPlugin(const RDEntryLoader* ploader);
RD_API_EXPORT rd_flag RDLoader_GetFlags(const RDLoader* ldr);
RD_API_EXPORT RDDocument* RDLoader_GetDocument(RDLoader* ldr);
RD_API_EXPORT RDDatabase* RDLoader_GetDatabase(RDLoader* ldr);
RD_API_EXPORT RDBuffer* RDLoader_GetBuffer(RDLoader* ldr);
RD_API_EXPORT u8* RDLoader_GetData(RDLoader* ldr);

RD_API_EXPORT u8* RD_AddrPointer(const RDLoader* ldr, rd_address address);
RD_API_EXPORT u8* RD_Pointer(const RDLoader* ldr, rd_offset offset);
RD_API_EXPORT RDLocation RD_FileOffset(const RDLoader* ldr, const void* ptr);
RD_API_EXPORT RDLocation RD_AddressOf(const RDLoader* ldr, const void* ptr);
RD_API_EXPORT RDLocation RD_Offset(const RDLoader* ldr, rd_address address);
RD_API_EXPORT RDLocation RD_Address(const RDLoader* ldr, rd_offset offset);
