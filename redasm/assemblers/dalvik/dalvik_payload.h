#ifndef DALVIK_PAYLOAD_H
#define DALVIK_PAYLOAD_H

#include "../../redasm.h"

#define DALVIK_PACKED_SWITCH_IDENT   0x0100
#define DALVIK_SPARSE_SWITCH_IDENT   0x0200
#define DALVIK_FILL_ARRAY_DATA_IDENT 0x0300

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
    u8 data[1];
};

} // namespace REDasm

VISITABLE_STRUCT(REDasm::DalvikPackedSwitchPayload, ident, size, first_key);
VISITABLE_STRUCT(REDasm::DalvikSparseSwitchPayload, ident, size);
VISITABLE_STRUCT(REDasm::DalvikFillArrayDataPayload, ident, element_width, size);

#endif // DALVIK_PAYLOAD_H
