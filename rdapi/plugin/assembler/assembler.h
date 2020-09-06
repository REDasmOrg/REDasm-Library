#pragma once

#include "../../macros.h"
#include "../../buffer.h"
#include "../../plugin.h"
#include "../../renderer.h"
#include "../../rdil.h"

#define ENCODE_BUFFER_SIZE 64

DECLARE_HANDLE(RDAssembler);

struct RDAssemblerPlugin;
struct RDEmulateResult;
struct RDDisassembler;

enum RDEncodeFlags {
    EncodeFlags_None    = 0,
    EncodeFlags_FillNop = (1 << 0),
};

typedef struct RDEncodedInstruction {
    const char* decoded;
    rd_flag flags;
    size_t count;
    u8 encoded[ENCODE_BUFFER_SIZE];
} RDEncodedInstruction;

RD_API_EXPORT void RDEncodedInstruction_Init(RDEncodedInstruction* encoded);
RD_API_EXPORT bool RDEncodedInstruction_Set(RDEncodedInstruction* encoded, u8* encdata, size_t count);

typedef void (*Callback_AssemblerPlugin)(struct RDAssemblerPlugin* plugin, void* userdata);
typedef void (*Callback_AssemblerEmulate)(const struct RDAssemblerPlugin* plugin, RDEmulateResult* result);
typedef void (*Callback_AssemblerRenderInstruction)(const RDAssemblerPlugin* plugin, const RDRenderItemParams* rip);
typedef void (*Callback_AssemblerLift)(const RDAssemblerPlugin* plugin, rd_address address, const RDBufferView* view, RDILFunction* il);

typedef size_t (*Callback_AssemblerEncode)(const struct RDAssemblerPlugin* plugin, RDEncodedInstruction* encoded);

typedef struct RDAssemblerPlugin {
    RD_PLUGIN_HEADER
    u32 bits;

    Callback_AssemblerRenderInstruction renderinstruction;
    Callback_AssemblerEmulate emulate;
    Callback_AssemblerLift lift;

    Callback_AssemblerEncode encode; // TODO: Review
} RDAssemblerPlugin;

RD_API_EXPORT const char* RDAssembler_GetId(const RDAssembler* assembler);
RD_API_EXPORT bool RDAssembler_GetUserData(const RDAssembler* assembler, RDUserData* userdata);

// Extra Functions
RD_API_EXPORT void RD_GetAssemblers(Callback_AssemblerPlugin callback, void* userdata);
RD_API_EXPORT bool RDAssembler_Register(RDAssemblerPlugin* plugin);
RD_API_EXPORT RDAssemblerPlugin* RDAssembler_Find(const char* name);
