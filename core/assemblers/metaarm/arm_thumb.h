#pragma once

#include "arm_common.h"

namespace REDasm {

class ARMThumbAssembler : public ARMCommonAssembler<CS_ARCH_ARM, CS_MODE_THUMB>
{
    PLUGIN_NAME("ARM Thumb")

    public:
        ARMThumbAssembler();
        u64 pc(const InstructionPtr& instruction) const override;
};

DECLARE_ASSEMBLER_PLUGIN(ARMThumbAssembler, armthumb, 16)

} // namespace REDasm
