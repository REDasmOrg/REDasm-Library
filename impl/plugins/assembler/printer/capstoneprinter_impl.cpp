#include "capstoneprinter_impl.h"
#include <redasm/plugins/assembler/capstoneassembler.h>

namespace REDasm {

CapstonePrinterImpl::CapstonePrinterImpl(Disassembler *disassembler): PrinterImpl(disassembler)
{
    m_handle = static_cast<CapstoneAssembler*>(disassembler->assembler())->handle();
}

} // namespace REDasm
