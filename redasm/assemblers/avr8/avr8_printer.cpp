#include "avr8_printer.h"

namespace REDasm {

AVR8Printer::AVR8Printer(DisassemblerAPI *disassembler): Printer(disassembler)
{

}

std::string AVR8Printer::reg(const RegisterOperand &regop) const
{
    return "r" + std::to_string(regop.r);
}

} // namespace REDasm
