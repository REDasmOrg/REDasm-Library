#ifndef DALVIK_PAYLOAD_H
#define DALVIK_PAYLOAD_H

#include "../../redasm.h"

namespace REDasm {

struct DalvikPackedSwitchPayload {
    u16 ident, size;
    u32 first_key;
    u32 targets[1];
};

struct DalvikSparseSwitchPayload {
    u16 ident, size;
    u32 keys[1];
    //u32 targets[1];
};

struct DalvikFillArrayDataPayload {
    u16 ident, element_width;
    u32 size;
    u8 data[];
};

} // namespace REDasm

VISITABLE_STRUCT(REDasm::DalvikPackedSwitchPayload, ident, size, first_key);
VISITABLE_STRUCT(REDasm::DalvikSparseSwitchPayload, ident, size);
VISITABLE_STRUCT(REDasm::DalvikFillArrayDataPayload, ident, element_width, size);

#endif // DALVIK_PAYLOAD_H
