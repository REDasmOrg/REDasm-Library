#pragma once

#include <capstone/capstone.h>
#include <redasm/plugins/assembler/printer/capstoneprinter.h>
#include "printer_impl.h"

namespace REDasm {

class CapstonePrinterImpl: public PrinterImpl
{
    PIMPL_DECLARE_PUBLIC(CapstonePrinter)

    public:
        CapstonePrinterImpl(Disassembler* disassembler);

    private:
        csh m_handle;
};

} // namespace REDasm
