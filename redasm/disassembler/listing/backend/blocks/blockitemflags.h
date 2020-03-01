#pragma once

#include "../../../types/base.h"
#include "../../../macros.h"

namespace REDasm {

DECLARE_TYPES(BlockItem, Unexplored, Data, Code)

namespace BlockItemFlags {

enum: flag_t {
    None        = 0,
    Function    = (1 << 0),
    Label       = (1 << 1),
    AsciiString = (1 << 2),
    WideString  = (1 << 3),
    Import      = (1 << 4),
    Export      = (1 << 5),
    EntryPoint  = (1 << 6),
    Pointer     = (1 << 7),
};

} // namespace BlockItemFlags

} // namespace REDasm
