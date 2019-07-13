#pragma once

#include <capstone/capstone.h>
#include <redasm/plugins/assembler/printer/capstoneprinter.h>

namespace REDasm {

class CapstonePrinterImpl
{
    PIMPL_DECLARE_Q(CapstonePrinter)
    PIMPL_DECLARE_PUBLIC(CapstonePrinter)

    public:
        CapstonePrinterImpl();

    private:
        csh m_handle;
};

} // namespace REDasm
