#include "metaarm_printer.h"
#include "../../plugins/loader.h"

namespace REDasm {

MetaARMPrinter::MetaARMPrinter(csh cshandle, DisassemblerAPI *disassembler): CapstonePrinter(cshandle, disassembler) { }
std::string MetaARMPrinter::size(const Operand *operand) const { RE_UNUSED(operand); return std::string(); }

std::string MetaARMPrinter::mem(const Operand *operand) const
{
    u64 value = 0;

    if(!m_disassembler->readAddress(operand->u_value, operand->size, &value))
        return CapstonePrinter::mem(operand);

    Symbol* symbol = m_document->symbol(value);
    return "=" + (symbol ? symbol->name : REDasm::hex(value, m_disassembler->loader()->bits()));
}

} // namespace REDasm
