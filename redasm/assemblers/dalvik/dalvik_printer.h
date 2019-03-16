#ifndef DALVIK_PRINTER_H
#define DALVIK_PRINTER_H

#include "../../plugins/assembler/printer.h"
#include "../../loaders/dex/dex_header.h"

namespace REDasm {

class DEXLoader;

class DalvikPrinter : public Printer
{
    public:
        DalvikPrinter(DisassemblerAPI* disassembler);
        virtual void function(const Symbol* symbol, const FunctionCallback &plgfunc);
        virtual std::string reg(const RegisterOperand &regop) const;
        virtual std::string imm(const Operand *op) const;
};

} // namespace REDasm

#endif // DALVIK_PRINTER_H
