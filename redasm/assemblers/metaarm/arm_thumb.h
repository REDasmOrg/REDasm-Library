#pragma once

#include "arm_common.h"

namespace REDasm {

class ARMThumbAssembler : public ARMCommonAssembler<CS_ARCH_ARM, CS_MODE_THUMB>
{
    PLUGIN_NAME("ARM Thumb")

    public:
        ARMThumbAssembler();
        virtual u64 pc(const InstructionPtr& instruction) const;
};

DECLARE_ASSEMBLER_PLUGIN(ARMThumbAssembler, armthumb, 16)

} // namespace REDasm
