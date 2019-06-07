#pragma once

#include "printer.h"

namespace REDasm {

class CapstonePrinterImpl;

class CapstonePrinter: public Printer
{
    PIMPL_DECLARE_PRIVATE(CapstonePrinter)

    public:
        CapstonePrinter(Disassembler* disassembler);

    protected:
        std::string reg(const RegisterOperand &regop) const override;
};

} // namespace REDasm
