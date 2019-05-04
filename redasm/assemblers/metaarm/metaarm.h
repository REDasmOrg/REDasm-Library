#pragma once

#include "../../plugins/plugins.h"
#include "arm.h"
#include "arm_thumb.h"

namespace REDasm {

class MetaARMAssembler: public AssemblerPlugin, public ARMAbstractAssembler
{
    PLUGIN_NAME("MetaARM")

    public:
        MetaARMAssembler();
        virtual ~MetaARMAssembler();
        u32 flags() const override;
        u32 bits() const override;
        Emulator* createEmulator(DisassemblerAPI *disassembler) const override;
        Printer* createPrinter(DisassemblerAPI *disassembler) const override;
        AssemblerAlgorithm* createAlgorithm(DisassemblerAPI *disassembler) override;
        bool decode(const BufferView& view, const InstructionPtr& instruction) override;
        u64 pc(const InstructionPtr& instruction) const override;

    public:
        ARMAssembler* armAssembler();
        ARMThumbAssembler* thumbAssembler();
        bool isPC(const Operand* op) const;
        bool isLR(const Operand* op) const;
        bool isArm() const;
        bool isThumb() const;
        void switchToThumb();
        void switchToArm();

    private:
        ARMAssembler* m_armassembler;
        ARMThumbAssembler* m_thumbassembler;
        AssemblerPlugin* m_assembler;
};

DECLARE_ASSEMBLER_PLUGIN(MetaARMAssembler, metaarm, 32)

} // namespace REDasm
