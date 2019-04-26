#pragma once

#include "../../redasm.h"
#include "coff_constants.h"

namespace REDasm {
namespace COFF {

struct COFF_Entry
{
    union {
        char e_name[E_SYMNMLEN];
        struct { u32 e_zeroes, e_offset; };
    };

    u32 e_value;
    s16 e_scnum;
    u16 e_type;
    s8 e_sclass;
    u8 e_numaux;
};

} // namespace COFF
} // namespace REDasm
