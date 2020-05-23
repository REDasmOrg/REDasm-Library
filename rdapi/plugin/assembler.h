#pragma once

#include "../macros.h"
#include "../buffer.h"
#include "../plugin.h"
#include "../renderer.h"

#define ENCODE_BUFFER_SIZE 64

struct RDAssemblerPlugin;
struct RDDisassembler;

enum RDEncodeFlags {
    EncodeFlags_None    = 0,
    EncodeFlags_FillNop = (1 << 0),
};

typedef struct RDEncodedInstruction {
    const char* decoded;
    flag_t flags;
    size_t count;
    u8 encoded[ENCODE_BUFFER_SIZE];
} RDEncodedInstruction;

RD_API_EXPORT void RDEncodedInstruction_Init(RDEncodedInstruction* encoded);
RD_API_EXPORT bool RDEncodedInstruction_Set(RDEncodedInstruction* encoded, u8* encdata, size_t count);

typedef const char* (*Callback_AssemblerRegName)(struct RDAssemblerPlugin* plugin, const RDInstruction* instruction, register_id_t r);
typedef size_t (*Callback_AssemblerEncode)(const struct RDAssemblerPlugin* plugin, RDEncodedInstruction* encoded);
typedef bool (*Callback_AssemblerDecode)(const struct RDAssemblerPlugin* plugin, RDBufferView* view, RDInstruction* instruction);
typedef void (*Callback_AssemblerEmulate)(const struct RDAssemblerPlugin* plugin, RDDisassembler* disassembler, const RDInstruction* instruction);
typedef void (*Callback_AssemblerPlugin)(struct RDAssemblerPlugin* plugin, void* userdata);
typedef bool (*Callback_AssemblerRender)(const RDAssemblerPlugin* plugin, RDRenderItemParams* rip);

typedef struct RDAssemblerPlugin {
    RD_PLUGIN_HEADER
    u32 bits;

    Callback_AssemblerRegName regname;

    Callback_AssemblerEncode encode;
    Callback_AssemblerDecode decode;
    Callback_AssemblerEmulate emulate;
    Callback_AssemblerRender render;
} RDAssemblerPlugin;

RD_API_EXPORT void RD_GetAssemblers(Callback_AssemblerPlugin callback, void* userdata);
RD_API_EXPORT bool RDAssembler_Register(RDAssemblerPlugin* plugin);
RD_API_EXPORT RDAssemblerPlugin* RDAssembler_Find(const char* name);
