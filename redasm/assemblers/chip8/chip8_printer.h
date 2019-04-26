#pragma once

#include "../../plugins/assembler/printer.h"

namespace REDasm {

class CHIP8Printer : public Printer
{
    public:
        CHIP8Printer(DisassemblerAPI* disassembler);
        virtual std::string reg(const RegisterOperand& regop) const;
};

} // namespace REDasm
