#pragma once

#include "../../plugins/assembler/printer.h"

namespace REDasm {

class MetaARMPrinter: public CapstonePrinter
{
    public:
        MetaARMPrinter(csh cshandle, DisassemblerAPI* disassembler);

    public:
        std::string size(const Operand* operand) const override;
        std::string mem(const Operand* operand) const override;
};

} // namespace REDasm
