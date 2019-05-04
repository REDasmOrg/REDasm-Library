#pragma once

#include "../../plugins/plugins.h"
#include "x86_printer.h"

#define X86_REGISTER(reg) ((reg == X86_REG_INVALID) ? REGISTER_INVALID : reg)

namespace REDasm {

template<cs_mode mode> class X86Assembler: public CapstoneAssemblerPlugin<CS_ARCH_X86, mode>
{
    public:
        X86Assembler();
        Printer* createPrinter(DisassemblerAPI *disassembler) const override { return new X86Printer(this->m_cshandle, disassembler); }

    protected:
        void onDecoded(const InstructionPtr& instruction) override;

    private:
        void setBranchTarget(const InstructionPtr& instruction);
        void checkLea(const InstructionPtr& instruction);
        void compareOp1(const InstructionPtr& instruction);
        s64 bpIndex(s64 disp, OperandType &type) const;
        s64 spIndex(s64 disp) const;
        bool isSP(register_id_t reg) const;
        bool isBP(register_id_t reg) const;
        bool isIP(register_id_t reg) const;
};

ASSEMBLER_INHERIT(X86_16Assembler, (X86Assembler<CS_MODE_16>), "x86_16")
ASSEMBLER_INHERIT(X86_32Assembler, (X86Assembler<CS_MODE_32>), "x86_32")
ASSEMBLER_INHERIT(X86_64Assembler, (X86Assembler<CS_MODE_64>), "x86_64")

DECLARE_ASSEMBLER_PLUGIN(X86_16Assembler, x86_16, 16)
DECLARE_ASSEMBLER_PLUGIN(X86_32Assembler, x86_32, 32)
DECLARE_ASSEMBLER_PLUGIN(X86_64Assembler, x86_64, 64)

} // namespace REDasm

#include "x86.cpp"
