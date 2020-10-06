#pragma once

#include "../macros.h"
#include "../types.h"
#include "entry.h"

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

struct RDEntryCommand;
struct RDDisassembler;

typedef bool (*Callback_CommandExecute)(const struct RDEntrycommand* plugin, const RDArguments* arguments);

typedef struct RDEntryCommand {
    RD_ENTRY_HEADER

    Callback_CommandExecute execute;
} RDEntryCommand;

RD_API_EXPORT bool RDCommand_Register(RDPluginModule* m, const RDEntryCommand* entry);
RD_API_EXPORT bool RDCommand_Execute(RDContext* ctx, const char* command, const RDArguments* arguments);
