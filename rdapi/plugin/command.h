#pragma once

#include "../macros.h"
#include "../types.h"
#include "../plugin.h"

#define RD_ARGUMENTS_SIZE 10

enum RDArgumentType {
    ArgumentType_Void,
    ArgumentType_Int,
    ArgumentType_UInt,
    ArgumentType_String,
    ArgumentType_Pointer,
};

typedef struct RDArgument {
    rd_type type;
    RD_USERDATA_FIELD
} RDArgument;

typedef struct RDArguments {
    u32 count;
    RDArgument args[RD_ARGUMENTS_SIZE];
} RDArguments;

RD_API_EXPORT void RDArguments_Init(RDArguments* arguments);
RD_API_EXPORT bool RDArguments_PushInt(RDArguments* arguments, intptr_t v);
RD_API_EXPORT bool RDArguments_PushUInt(RDArguments* arguments, uintptr_t v);
RD_API_EXPORT bool RDArguments_PushString(RDArguments* arguments, const char* v);
RD_API_EXPORT bool RDArguments_PushPointer(RDArguments* arguments, void* v);

struct RDCommandPlugin;
struct RDDisassembler;

typedef bool (*Callback_CommandExecute)(const struct RDCommandPlugin* plugin, const RDArguments* arguments);

typedef struct RDCommandPlugin {
    RD_PLUGIN_HEADER

    Callback_CommandExecute execute;
} RDCommandPlugin;

RD_API_EXPORT bool RDCommand_Register(RDCommandPlugin* plugin);
RD_API_EXPORT bool RDCommand_Execute(const char* command, const RDArguments* arguments);
