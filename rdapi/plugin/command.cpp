#include "command.h"
#include <rdcore/context.h>

static RDArgument* RDArguments_Push(RDArguments* arguments, type_t type)
{
    if(arguments->count >= RD_ARGUMENTS_SIZE) return nullptr;

    RDArgument* arg = &arguments->args[arguments->count];
    arg->type = type;
    arguments->count++;
    return arg;
}

void RDArguments_Init(RDArguments* arguments) { *arguments = { }; }

bool RDArguments_PushInt(RDArguments* arguments, intptr_t v)
{
    if(auto* a = RDArguments_Push(arguments, ArgumentType_Int))
    {
        a->i_data = v;
        return true;
    }

    return false;
}

bool RDArguments_PushUInt(RDArguments* arguments, uintptr_t v)
{
    if(auto* a = RDArguments_Push(arguments, ArgumentType_UInt))
    {
        a->u_data = v;
        return true;
    }

    return false;
}

bool RDArguments_PushString(RDArguments* arguments, const char* v)
{
    if(auto* a = RDArguments_Push(arguments, ArgumentType_String))
    {
        a->s_data = v;
        return true;
    }

    return false;
}

bool RDArguments_PushPointer(RDArguments* arguments, void* v)
{
    if(auto* a = RDArguments_Push(arguments, ArgumentType_Pointer))
    {
        a->p_data = v;
        return true;
    }

    return false;
}

bool RDCommand_Register(RDCommandPlugin* plugin) { return rd_ctx->registerPlugin(plugin); }
bool RDCommand_Execute(const char* command, const RDArguments* arguments) { return rd_ctx->commandExecute(command, arguments); }
