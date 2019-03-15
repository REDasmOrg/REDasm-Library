#ifndef AVR8_PRINTER_H
#define AVR8_PRINTER_H

#include "../../plugins/assembler/printer.h"

namespace REDasm {

class AVR8Printer: public Printer
{
    public:
        AVR8Printer(DisassemblerAPI* disassembler);
};

} // namespace REDasm

#endif // AVR8_PRINTER_H
