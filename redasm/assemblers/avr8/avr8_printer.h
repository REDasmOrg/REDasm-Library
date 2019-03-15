#ifndef AVR8_PRINTER_H
#define AVR8_PRINTER_H

#include "../../plugins/assembler/printer.h"

namespace REDasm {

class AVR8Printer: public Printer
{
    public:
        AVR8Printer(DisassemblerAPI* disassembler);
        virtual std::string reg(const RegisterOperand &regop) const;
};

} // namespace REDasm

#endif // AVR8_PRINTER_H
