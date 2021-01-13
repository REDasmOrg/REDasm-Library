#pragma once

#include "../macros.h"
#include "../types.h"
#include "entry.h"

#define RD_ARGUMENTS_SIZE 10

enum RDArgumentType {
    ArgumentType_Void,
    ArgumentType_Int,     // i
    ArgumentType_UInt,    // u
    ArgumentType_String,  // s
    ArgumentType_Pointer, // p
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

struct RDEntryCommand;

typedef bool (*Callback_CommandIsEnabled)(const struct RDContext* ctx);
typedef bool (*Callback_CommandExecute)(struct RDContext* ctx, const RDArguments* arguments);

typedef struct RDEntryCommand {
    RD_ENTRY_HEADER

    const char* signature;
    Callback_CommandIsEnabled isenabled;
    Callback_CommandExecute execute;
} RDEntryCommand;

RD_API_EXPORT bool RDCommand_Register(RDPluginModule* pm, const RDEntryCommand* entry);
