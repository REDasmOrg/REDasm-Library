#pragma once

#include "../../plugins/assembler/printer.h"

namespace REDasm {

class AVR8Printer: public Printer
{
    public:
        AVR8Printer(DisassemblerAPI* disassembler);
        std::string reg(const RegisterOperand &regop) const override;
};

} // namespace REDasm
