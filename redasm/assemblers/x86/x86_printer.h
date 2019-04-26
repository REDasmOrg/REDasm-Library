#pragma once

#include "../../plugins/assembler/printer.h"

namespace REDasm {

class X86Printer: public CapstonePrinter
{
    public:
        X86Printer(csh cshandle, DisassemblerAPI *disassembler);
        virtual std::string loc(const Operand *op) const;
};

} // namespace REDasm
