#pragma once

#include <redasm/plugins/assembler/printer/printer.h>

namespace REDasm {

class PrinterImpl
{
    PIMPL_DECLARE_PUBLIC(Printer)

    public:
        PrinterImpl(Disassembler* disassembler);

    private:
        Disassembler* m_disassembler;
        ListingDocument& m_document;
};

} // namespace REDasm
