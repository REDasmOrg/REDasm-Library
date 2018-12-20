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
        virtual const char* name() const;
        virtual u32 flags() const { return AssemblerFlags::HasEmulator; }
        virtual Emulator* createEmulator(DisassemblerAPI *disassembler) const { return new MIPSEmulator(disassembler); }
        virtual Printer* createPrinter(DisassemblerAPI* disassembler) const { return new MIPSPrinter(this->m_cshandle, disassembler); }
        virtual AssemblerAlgorithm* createAlgorithm(DisassemblerAPI* disassembler) { return new MIPSAlgorithm(disassembler, this); }

    protected:
        virtual bool decodeInstruction(BufferRef& buffer, const InstructionPtr& instruction);
        virtual void onDecoded(const InstructionPtr& instruction);

    private:
        void setTargetOp0(const InstructionPtr& instruction) const { instruction->targetOp(0); }
        void setTargetOp1(const InstructionPtr& instruction) const { instruction->targetOp(1); }
        void setTargetOp2(const InstructionPtr& instruction) const { instruction->targetOp(2); }
        void checkJr(const InstructionPtr& instruction) const;
};

typedef MIPSAssembler<CS_MODE_MIPS32> MIPS32LEAssembler;
typedef MIPSAssembler<CS_MODE_MIPS64> MIPS64LEAssembler;

typedef MIPSAssembler<CS_MODE_MIPS32 | CS_MODE_BIG_ENDIAN> MIPS32BEAssembler;
typedef MIPSAssembler<CS_MODE_MIPS64 | CS_MODE_BIG_ENDIAN> MIPS64BEAssembler;

DECLARE_ASSEMBLER_PLUGIN(MIPS32LEAssembler, mips32le)
DECLARE_ASSEMBLER_PLUGIN(MIPS64LEAssembler, mips64le)

DECLARE_ASSEMBLER_PLUGIN(MIPS32BEAssembler, mips32be)
DECLARE_ASSEMBLER_PLUGIN(MIPS64BEAssembler, mips64be)

} // namespace REDasm

#include "mips.cpp"

#endif // MIPS_H
