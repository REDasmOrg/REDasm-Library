#pragma once

#include "../../plugins/assembler/printer.h"

namespace REDasm {

class MIPSPrinter : public CapstonePrinter
{
    public:
        MIPSPrinter(csh cshandle, DisassemblerAPI* disassembler);
        virtual std::string reg(const RegisterOperand& regop) const;
        virtual std::string disp(const Operand* operand) const;
};

} // namespace REDasm
