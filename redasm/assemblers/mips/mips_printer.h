#pragma once

#include "../../plugins/assembler/printer.h"

namespace REDasm {

class MIPSPrinter : public CapstonePrinter
{
    public:
        MIPSPrinter(csh cshandle, DisassemblerAPI* disassembler);
        std::string reg(const RegisterOperand& regop) const override;
        std::string disp(const Operand* operand) const override;
};

} // namespace REDasm
