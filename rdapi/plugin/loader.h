#pragma once

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

RD_HANDLE(RDLoader);

struct RDEntryLoader;
struct RDContext;

typedef const char* (*Callback_LoaderTest)(const RDLoaderRequest* request);
typedef bool (*Callback_LoaderLoad)(struct RDContext* ctx);
typedef bool (*Callback_LoaderBuild)(struct RDContext* ctx, const RDLoaderBuildParams* buildparams);

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
RD_API_EXPORT rd_endianness RDLoader_GetEndianness(const RDLoader* ldr);
RD_API_EXPORT rd_flag RDLoader_GetFlags(const RDLoader* ldr);
