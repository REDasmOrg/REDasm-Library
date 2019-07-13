#include "capstoneprinter_impl.h"
#include <redasm/plugins/assembler/capstoneassembler.h>
#include <redasm/disassembler/disassembler.h>
#include <redasm/context.h>

namespace REDasm {

CapstonePrinterImpl::CapstonePrinterImpl()
{
    m_handle = static_cast<CapstoneAssembler*>(r_asm)->handle();
}

} // namespace REDasm
