#pragma once

#include <cstddef>

namespace REDasm {
namespace StandardEvents {

enum: size_t {
    None = 0,

    // Standard Events
    Disassembler_BusyChanged,
    Document_Changed,
    Document_BlockInserted,
    Document_BlockErased,
    Cursor_PositionChanged,
    Cursor_BackChanged,
    Cursor_ForwardChanged,

    // User defined Events
    UserFirst = (1u << 31),
};

}

} // namespace REDasm
