#pragma once

#include "../../plugins/plugins.h"
#include "mips_printer.h"
#include "mips_emulator.h"
#include "mips_quirks.h"
#include "mips_algorithm.h"

namespace REDasm {

template<s64 mode> class MIPSAssembler: public CapstoneAssemblerPlugin<CS_ARCH_MIPS, mode>
{
    public:
        MIPSAssembler();
        u32 flags() const override { return AssemblerFlags::CanEmulate; }
        Emulator* createEmulator(DisassemblerAPI *disassembler) const override { return new MIPSEmulator(disassembler); }
        Printer* createPrinter(DisassemblerAPI* disassembler) const override { return new MIPSPrinter(this->m_cshandle, disassembler); }
        AssemblerAlgorithm* createAlgorithm(DisassemblerAPI* disassembler) override { return new MIPSAlgorithm(disassembler, this); }

    protected:
        bool decodeInstruction(const BufferView &view, const InstructionPtr& instruction) override;
        void onDecoded(const InstructionPtr& instruction) override;

    private:
        void setTargetOp0(const InstructionPtr& instruction) const { instruction->targetIdx(0); }
        void setTargetOp1(const InstructionPtr& instruction) const { instruction->targetIdx(1); }
        void setTargetOp2(const InstructionPtr& instruction) const { instruction->targetIdx(2); }
        void checkJr(const InstructionPtr& instruction) const;
};

ASSEMBLER_INHERIT(MIPS32LEAssembler, (MIPSAssembler<CS_MODE_MIPS32>), "MIPS 32 LE")
ASSEMBLER_INHERIT(MIPS64LEAssembler, (MIPSAssembler<CS_MODE_MIPS64>), "MIPS 64 LE")
ASSEMBLER_INHERIT(MIPS32R6LEAssembler, (MIPSAssembler<CS_MODE_MIPS32R6>), "MIPS 32r6 LE")
ASSEMBLER_INHERIT(MIPS2LEAssembler, (MIPSAssembler<CS_MODE_MIPS2>), "MIPS II LE")
ASSEMBLER_INHERIT(MIPS3LEAssembler, (MIPSAssembler<CS_MODE_MIPS3>), "MIPS III LE")
ASSEMBLER_INHERIT(MIPSMicroLEAssembler, (MIPSAssembler<CS_MODE_MICRO>), "Micro MIPS LE")

ASSEMBLER_INHERIT(MIPS32BEAssembler, (MIPSAssembler<CS_MODE_MIPS32 | CS_MODE_BIG_ENDIAN>), "MIPS 32 BE")
ASSEMBLER_INHERIT(MIPS64BEAssembler, (MIPSAssembler<CS_MODE_MIPS64 | CS_MODE_BIG_ENDIAN>), "MIPS 64 BE")
ASSEMBLER_INHERIT(MIPS32R6BEAssembler, (MIPSAssembler<CS_MODE_MIPS32R6 | CS_MODE_BIG_ENDIAN>), "MIPS 32r6 BE")
ASSEMBLER_INHERIT(MIPS2BEAssembler, (MIPSAssembler<CS_MODE_MIPS2 | CS_MODE_BIG_ENDIAN>), "MIPS II BE")
ASSEMBLER_INHERIT(MIPS3BEAssembler, (MIPSAssembler<CS_MODE_MIPS3 | CS_MODE_BIG_ENDIAN>), "MIPS III BE")
ASSEMBLER_INHERIT(MIPSMicroBEAssembler, (MIPSAssembler<CS_MODE_MICRO | CS_MODE_BIG_ENDIAN>), "Micro MIPS BE")

DECLARE_ASSEMBLER_PLUGIN(MIPS32LEAssembler, mips32le, 32)
DECLARE_ASSEMBLER_PLUGIN(MIPS64LEAssembler, mips64le, 64)
DECLARE_ASSEMBLER_PLUGIN(MIPS32R6LEAssembler, mips32r6le, 32)
DECLARE_ASSEMBLER_PLUGIN(MIPS2LEAssembler, mips2le, 32)
DECLARE_ASSEMBLER_PLUGIN(MIPS3LEAssembler, mips3le, 32)
DECLARE_ASSEMBLER_PLUGIN(MIPSMicroLEAssembler, mipsmicrole, 32)

DECLARE_ASSEMBLER_PLUGIN(MIPS32BEAssembler, mips32be, 32)
DECLARE_ASSEMBLER_PLUGIN(MIPS64BEAssembler, mips64be, 64)
DECLARE_ASSEMBLER_PLUGIN(MIPS32R6BEAssembler, mips32r6be, 32)
DECLARE_ASSEMBLER_PLUGIN(MIPS2BEAssembler, mips2be, 32)
DECLARE_ASSEMBLER_PLUGIN(MIPS3BEAssembler, mips3be, 32)
DECLARE_ASSEMBLER_PLUGIN(MIPSMicroBEAssembler, mipsmicrobe, 32)

} // namespace REDasm

#include "mips.hpp"
