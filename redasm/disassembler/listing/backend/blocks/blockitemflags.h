#pragma once

#include "../../../macros.h"

namespace REDasm {

enum class BlockItemType {
    Unexplored,
    Data,
    Code,
};

enum class BlockItemFlags {
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

ENUM_FLAGS_OPERATORS(BlockItemFlags);

} // namespace REDasm
