#pragma once

#include "../../plugins/assembler/printer.h"

namespace REDasm {

class AVR8Printer: public Printer
{
    public:
        AVR8Printer(DisassemblerAPI* disassembler);
        virtual std::string reg(const RegisterOperand &regop) const;
};

} // namespace REDasm
