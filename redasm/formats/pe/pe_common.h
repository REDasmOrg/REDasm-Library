#ifndef PE_COMMON_H
#define PE_COMMON_H

#include "../../redasm.h"

namespace REDasm {

struct GUID
{
    u32 data1;
    u16 data2, data3;
    u8 data4[8];
};

typedef u32 LCID;

} // namespace REDasm

VISITABLE_STRUCT(REDasm::GUID, data1, data2, data3, data4);

#endif // PE_COMMON_H
