#pragma once

#include "../../plugins/plugins.h"
#include "arm_common.h"
#include "metaarm_printer.h"

namespace REDasm {

class ARMAssembler: public ARMCommonAssembler<CS_ARCH_ARM, CS_MODE_ARM>
{
    PLUGIN_NAME("ARM")

    public:
        ARMAssembler();
        virtual ~ARMAssembler() = default;
        u32 flags() const override;
        Emulator* createEmulator(DisassemblerAPI *disassembler) const override;
        Printer* createPrinter(DisassemblerAPI *disassembler) const override;
        u64 pc(const InstructionPtr& instruction) const override;
};

DECLARE_ASSEMBLER_PLUGIN(ARMAssembler, arm, 32)

} // namespace REDasm
