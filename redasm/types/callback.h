#pragma once

#include <cstddef>

namespace REDasm {

class Context;
class PluginDescriptor;
class ArgumentList;

typedef bool (*Callback_ReturnBool)();
typedef void (*Callback_ReturnVoid)();
typedef const char*(*Callback_ReturnString)();
typedef size_t(*Callback_ReturnInteger)();
typedef PluginDescriptor*(*Callback_PluginInit)(Context*);
typedef bool(*Callback_PluginExec)(const ArgumentList&);
typedef void(*Callback_VoidPointer)(void*);

} // namespace REDasm
