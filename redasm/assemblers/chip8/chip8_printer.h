#pragma once

#include "../../plugins/assembler/printer.h"

namespace REDasm {

class CHIP8Printer : public Printer
{
    public:
        CHIP8Printer(DisassemblerAPI* disassembler);
        std::string reg(const RegisterOperand& regop) const override;
};

} // namespace REDasm
