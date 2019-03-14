#ifndef METAARM_H
#define METAARM_H

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
        virtual u32 flags() const;
        virtual Emulator* createEmulator(DisassemblerAPI *disassembler) const;
        virtual Printer* createPrinter(DisassemblerAPI *disassembler) const;
        virtual AssemblerAlgorithm* createAlgorithm(DisassemblerAPI *disassembler);
        virtual bool decode(const BufferView& view, const InstructionPtr& instruction);
        virtual u64 pc(const InstructionPtr& instruction) const;

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

DECLARE_ASSEMBLER_PLUGIN(MetaARMAssembler, metaarm)

} // namespace REDasm

#endif // METAARM_H
