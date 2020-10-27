#pragma once

#include "../../macros.h"
#include "../../buffer.h"
#include "../../renderer/renderer.h"
#include "../../rdil.h"
#include "../entry.h"

#define ENCODE_BUFFER_SIZE 64

DECLARE_HANDLE(RDAssembler);

struct RDEntryAssembler;
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

typedef void (*Callback_AssemblerEmulate)(RDContext* ctx, RDEmulateResult* result);
typedef void (*Callback_AssemblerRenderInstruction)(RDContext* ctx, const RDRendererParams* rp);
typedef void (*Callback_AssemblerLift)(RDContext* ctx, rd_address address, const RDBufferView* view, RDILFunction* il);

typedef size_t (*Callback_AssemblerEncode)(const struct RDEntryAssembler* entry, RDEncodedInstruction* encoded);

typedef struct RDEntryAssembler {
    RD_ENTRY_HEADER
    u32 bits;

    Callback_AssemblerRenderInstruction renderinstruction;
    Callback_AssemblerEmulate emulate;
    Callback_AssemblerLift lift;

    Callback_AssemblerEncode encode; // TODO: Review
} RDEntryAssembler;

RD_API_EXPORT bool RDAssembler_Register(RDPluginModule* m, const RDEntryAssembler* entry);
RD_API_EXPORT const char* RDAssembler_GetId(const RDAssembler* assembler);
RD_API_EXPORT const char* RDAssembler_GetName(const RDAssembler* assembler);
