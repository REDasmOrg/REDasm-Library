#ifndef MIPS_H
#define MIPS_H

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
        virtual std::string name() const;
        virtual u32 flags() const { return AssemblerFlags::HasEmulator; }
        virtual Emulator* createEmulator(DisassemblerAPI *disassembler) const { return new MIPSEmulator(disassembler); }
        virtual Printer* createPrinter(DisassemblerAPI* disassembler) const { return new MIPSPrinter(this->m_cshandle, disassembler); }
        virtual AssemblerAlgorithm* createAlgorithm(DisassemblerAPI* disassembler) { return new MIPSAlgorithm(disassembler, this); }

    protected:
        virtual bool decodeInstruction(const BufferRef &buffer, const InstructionPtr& instruction);
        virtual void onDecoded(const InstructionPtr& instruction);

    private:
        void setTargetOp0(const InstructionPtr& instruction) const { instruction->targetOp(0); }
        void setTargetOp1(const InstructionPtr& instruction) const { instruction->targetOp(1); }
        void setTargetOp2(const InstructionPtr& instruction) const { instruction->targetOp(2); }
        void checkJr(const InstructionPtr& instruction) const;
};

typedef MIPSAssembler<CS_MODE_MIPS32> MIPS32LEAssembler;
typedef MIPSAssembler<CS_MODE_MIPS64> MIPS64LEAssembler;
typedef MIPSAssembler<CS_MODE_MIPS32R6> MIPS32R6LEAssembler;
typedef MIPSAssembler<CS_MODE_MIPS2> MIPS2LEAssembler;
typedef MIPSAssembler<CS_MODE_MIPS3> MIPS3LEAssembler;
typedef MIPSAssembler<CS_MODE_MICRO> MIPSMicroLEAssembler;

typedef MIPSAssembler<CS_MODE_MIPS32 | CS_MODE_BIG_ENDIAN> MIPS32BEAssembler;
typedef MIPSAssembler<CS_MODE_MIPS64 | CS_MODE_BIG_ENDIAN> MIPS64BEAssembler;
typedef MIPSAssembler<CS_MODE_MIPS32R6 | CS_MODE_BIG_ENDIAN> MIPS32R6BEAssembler;
typedef MIPSAssembler<CS_MODE_MIPS2 | CS_MODE_BIG_ENDIAN> MIPS2BEAssembler;
typedef MIPSAssembler<CS_MODE_MIPS3 | CS_MODE_BIG_ENDIAN> MIPS3BEAssembler;
typedef MIPSAssembler<CS_MODE_MICRO | CS_MODE_BIG_ENDIAN> MIPSMicroBEAssembler;

DECLARE_ASSEMBLER_PLUGIN(MIPS32LEAssembler, mips32le)
DECLARE_ASSEMBLER_PLUGIN(MIPS64LEAssembler, mips64le)
DECLARE_ASSEMBLER_PLUGIN(MIPS32R6LEAssembler, mips32r6le)
DECLARE_ASSEMBLER_PLUGIN(MIPS2LEAssembler, mips2le)
DECLARE_ASSEMBLER_PLUGIN(MIPS3LEAssembler, mips3le)
DECLARE_ASSEMBLER_PLUGIN(MIPSMicroLEAssembler, mipsmicrole)

DECLARE_ASSEMBLER_PLUGIN(MIPS32BEAssembler, mips32be)
DECLARE_ASSEMBLER_PLUGIN(MIPS64BEAssembler, mips64be)
DECLARE_ASSEMBLER_PLUGIN(MIPS32R6BEAssembler, mips32r6be)
DECLARE_ASSEMBLER_PLUGIN(MIPS2BEAssembler, mips2be)
DECLARE_ASSEMBLER_PLUGIN(MIPS3BEAssembler, mips3be)
DECLARE_ASSEMBLER_PLUGIN(MIPSMicroBEAssembler, mipsmicrobe)

} // namespace REDasm

#include "mips_impl.h"

#endif // MIPS_H
