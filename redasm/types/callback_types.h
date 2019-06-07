#pragma once

#include <cstddef>

namespace REDasm {

class Context;
class PluginDescriptor;

typedef bool (*Callback_ReturnBool)();
typedef void (*Callback_ReturnVoid)();
typedef const char*(*Callback_ReturnString)();
typedef size_t(*Callback_ReturnInteger)();
typedef PluginDescriptor*(*Callback_PluginInit)(Context*);

} // namespace REDasm
