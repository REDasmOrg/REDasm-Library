#pragma once

#include "../document/document.h"
#include "../plugin.h"
#include "../buffer.h"
#include "assembler.h"

enum RDLoaderFlags {
    LoaderFlags_None               = 0,
    LoaderFlags_CustomAssembler    = (1 << 0),
    LoaderFlags_CustomAddressing   = (1 << 1),
    LoaderFlags_SkipUnexploredCode = (1 << 2),

    LoaderFlags_Binary             = -1
};

typedef struct RDLoaderRequest {
    const char* filepath;
    RDBuffer* buffer;
} RDLoaderRequest;

typedef struct RDLoaderBuildRequest {
    offset_t offset;
    address_t baseaddress;
    address_t entrypoint;
} RDLoaderBuildRequest;

DECLARE_HANDLE(RDLoader);

struct RDLoaderPlugin;

typedef const char* (*Callback_LoaderTest)(const struct RDLoaderPlugin* plugin, const RDLoaderRequest* request);
typedef void (*Callback_LoaderLoad)(struct RDLoaderPlugin* ploader, RDLoader* loader);
typedef void (*Callback_LoaderBuild)(struct RDLoaderPlugin* ploader, const RDLoaderBuildRequest* req);
typedef void (*Callback_LoaderAnalyze)(struct RDLoaderPlugin* ploader, RDDisassembler* disassembler);

typedef struct RDLoaderPlugin {
    RD_PLUGIN_HEADER
    flag_t flags;

    Callback_LoaderTest test;
    Callback_LoaderLoad load;
    Callback_LoaderBuild build;
    Callback_LoaderAnalyze analyze;
} RDLoaderPlugin;

typedef void (*Callback_LoaderPlugin)(RDLoaderPlugin* ploader, void* userdata);

RD_API_EXPORT bool RDLoader_Register(RDLoaderPlugin* ploader);
RD_API_EXPORT const char* RDLoader_GetAssemblerId(const RDLoaderPlugin* ploader);
RD_API_EXPORT RDAssemblerPlugin* RDLoader_GetAssembler(const RDLoaderPlugin* ploader);
RD_API_EXPORT flag_t RDLoader_GetFlags(const RDLoader* ldr);
RD_API_EXPORT RDDocument* RDLoader_GetDocument(RDLoader* ldr);
RD_API_EXPORT RDBuffer* RDLoader_GetBuffer(RDLoader* ldr);
RD_API_EXPORT u8* RDLoader_GetData(RDLoader* ldr);
RD_API_EXPORT void RDLoader_Build(RDLoader* ldr, const RDLoaderBuildRequest* req);
RD_API_EXPORT void RDLoader_Load(RDLoader* ldr);

RD_API_EXPORT void RD_GetLoaders(const RDLoaderRequest* loadrequest, Callback_LoaderPlugin callback, void* userdata);
RD_API_EXPORT u8* RD_AddrPointer(const RDLoader* ldr, address_t address);
RD_API_EXPORT u8* RD_Pointer(const RDLoader* ldr, offset_t offset);
RD_API_EXPORT RDLocation RD_FileOffset(const RDLoader* ldr, const void* ptr);
RD_API_EXPORT RDLocation RD_AddressOf(const RDLoader* ldr, const void* ptr);
RD_API_EXPORT RDLocation RD_Offset(const RDLoader* ldr, address_t address);
RD_API_EXPORT RDLocation RD_Address(const RDLoader* ldr, offset_t offset);
