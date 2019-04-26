#pragma once

#include "../../redasm.h"

namespace REDasm {

namespace DalvikOperands {
enum: u32 { Normal = 0, MethodIndex, TypeIndex, StringIndex, FieldIndex,
            PackedSwitchTable, SparseSwitchTable, FillArrayData,
            ParameterFirst = 0x1000, ParameterLast = 0x2000, ParameterThis = 0x4000 };
}

} // namespace REDasm
