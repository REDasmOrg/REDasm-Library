#include "printer_impl.h"

namespace REDasm {

PrinterImpl::PrinterImpl(Disassembler* disassembler): m_disassembler(disassembler), m_document(disassembler->document()) { }

} // namespace REDasm
