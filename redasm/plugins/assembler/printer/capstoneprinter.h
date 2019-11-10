#pragma once

#include "printer.h"

namespace REDasm {

class CapstonePrinterImpl;

class CapstonePrinter: public Printer
{
    PIMPL_DECLARE_P(CapstonePrinter)
    PIMPL_DECLARE_PRIVATE(CapstonePrinter)

    public:
        CapstonePrinter();

    protected:
        String reg(const RegisterOperand* regop) const override;
};

} // namespace REDasm
