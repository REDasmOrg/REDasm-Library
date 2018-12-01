#include "mips_printer.h"
#include "mips_quirks.h"

namespace REDasm {

MIPSPrinter::MIPSPrinter(csh cshandle, DisassemblerAPI *disassembler): CapstonePrinter(cshandle, disassembler)
{

}

std::string MIPSPrinter::reg(const RegisterOperand &regop) const
{
    if(regop.extra_type & MIPSRegisterTypes::Cop2Register)
        return "$" + REDasm::dec(regop.r);

    return "$" + CapstonePrinter::reg(regop);
}

std::string MIPSPrinter::disp(const Operand &operand) const
{
    return REDasm::hex(operand.disp.displacement) + "(" + this->reg(operand.disp.base) + ")";
}

}
