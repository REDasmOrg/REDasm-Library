#include "printer_impl.h"
#include <redasm/disassembler/disassembler.h>

namespace REDasm {

PrinterImpl::PrinterImpl(Disassembler* disassembler): m_disassembler(disassembler), m_document(disassembler->document()) { }

} // namespace REDasm
