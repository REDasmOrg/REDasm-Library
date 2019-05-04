#pragma once

#include "../../plugins/assembler/printer.h"
#include "../../loaders/dex/dex_header.h"

namespace REDasm {

class DEXLoader;

class DalvikPrinter : public Printer
{
    public:
        DalvikPrinter(DisassemblerAPI* disassembler);
        void function(const Symbol* symbol, const FunctionCallback &plgfunc) override;
        std::string reg(const RegisterOperand &regop) const override;
        std::string imm(const Operand *op) const override;
};

} // namespace REDasm
