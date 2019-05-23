#include "x86_printer.h"

namespace REDasm {

X86Printer::X86Printer(csh cshandle, DisassemblerAPI *disassembler): CapstonePrinter(cshandle, disassembler) { }

std::string X86Printer::loc(const Operand* op) const
{
    if(op->is(OperandType::Local))
        return "local_" + REDasm::hex(op->loc_index);
    if(op->is(OperandType::Argument))
        return "arg_" + REDasm::hex(op->loc_index);

    return std::string();
}

} // namespace REDasm
