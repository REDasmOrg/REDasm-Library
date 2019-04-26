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
        virtual u32 flags() const;
        virtual Emulator* createEmulator(DisassemblerAPI *disassembler) const;
        virtual Printer* createPrinter(DisassemblerAPI *disassembler) const;
        virtual u64 pc(const InstructionPtr& instruction) const;
};

DECLARE_ASSEMBLER_PLUGIN(ARMAssembler, arm, 32)

} // namespace REDasm
