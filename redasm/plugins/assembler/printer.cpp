#include "printer.h"
#include "../../plugins/format.h"
#include <cmath>

#define HEADER_SYMBOL_COUNT 10

namespace REDasm {

Printer::Printer(DisassemblerAPI *disassembler): m_document(disassembler->document()), m_disassembler(disassembler) { }

std::string Printer::symbol(const Symbol* symbol) const
{
    if(symbol->is(SymbolTypes::Pointer))
        return symbol->name;

    std::string s;

    this->symbol(symbol, [&s](const Symbol*, std::string line) {
        s = line;
    });

    return s;
}

std::string Printer::out(const InstructionPtr &instruction) const
{
    return this->out(instruction, [](const Operand*, const std::string&, const std::string&) { });
}

void Printer::segment(const Segment *segment, const Printer::LineCallback& segmentfunc)
{
    std::string s(HEADER_SYMBOL_COUNT * 2, '=');
    int bits = m_disassembler->format()->bits();

    segmentfunc(s + " SEGMENT " + (segment ? REDasm::quoted(segment->name) : "???") +
                    " START: " + REDasm::hex(segment->address, bits) +
                    " END: " + REDasm::hex(segment->endaddress, bits) + " " + s);
}

void Printer::function(const Symbol* symbol, const Printer::FunctionCallback& functionfunc)
{
    std::string s(HEADER_SYMBOL_COUNT, '=');
    functionfunc(s + " FUNCTION ", symbol->name, " " + s);
}

void Printer::prologue(const Symbol* symbol, const LineCallback &prologuefunc)
{
    RE_UNUSED(symbol);
    RE_UNUSED(prologuefunc);
}

void Printer::symbol(const Symbol* symbol, const SymbolCallback &symbolfunc) const
{
    if(symbol->isFunction() || symbol->is(SymbolTypes::Code))
        return;

    const Segment* segment = m_disassembler->document()->segment(symbol->address);

    if(!segment)
        return;

    if(symbol->is(SymbolTypes::Pointer))
    {
        const Symbol* ptrsymbol = m_disassembler->dereferenceSymbol(symbol);

        if(ptrsymbol)
        {
            symbolfunc(symbol, ptrsymbol->name);
            this->symbol(ptrsymbol, symbolfunc); // Emit pointed symbol too
            return;
        }
    }

    if(symbol->is(SymbolTypes::Data))
    {
        if(segment->is(SegmentTypes::Bss))
        {
            symbolfunc(symbol, "??");
            return;
        }

        FormatPlugin* formatplugin = m_disassembler->format();
        u64 value = 0;

        if(!m_disassembler->readAddress(symbol->address, formatplugin->addressWidth(), &value))
            return;

        symbolfunc(symbol, REDasm::hex(value, formatplugin->addressWidth()));
    }
    else if(symbol->is(SymbolTypes::WideStringMask))
        symbolfunc(symbol, " \"" + m_disassembler->readWString(symbol->address) + "\"");
    else if(symbol->is(SymbolTypes::String))
        symbolfunc(symbol, " \"" + m_disassembler->readString(symbol->address) + "\"");
}

void Printer::info(const InstructionPtr &instruction, const LineCallback &infofunc)
{
    RE_UNUSED(instruction);
    RE_UNUSED(infofunc);
}

std::string Printer::out(const InstructionPtr &instruction, const OpCallback &opfunc) const
{
    const OperandList& operands = instruction->operands;
    std::string s = instruction->mnemonic;

    if(instruction->isInvalid())
    {
        BufferView view = m_disassembler->format()->view(instruction->address);
        std::string hexstring = REDasm::hexstring(view, instruction->size);

        s += hexstring;
        opfunc(nullptr, std::string(), hexstring);
        return s;
    }

    if(!operands.empty())
        s += " ";

    for(auto it = operands.begin(); it != operands.end(); it++)
    {
        if(it != operands.begin())
            s += ", ";

        std::string opstr;
        const Operand& operand = *it;

        if(operand.is(OperandTypes::Immediate))
            opstr = this->imm(&operand);
        else if(operand.is(OperandTypes::Memory))
            opstr = this->mem(&operand);
        else if(operand.is(OperandTypes::Displacement))
            opstr = this->disp(&operand);
        else if(operand.is(OperandTypes::Register))
            opstr = this->reg(operand.reg);
        else
            continue;

        std::string opsize = this->size(&operand);

        if(opfunc)
            opfunc(&operand, opsize, opstr);

        if(!opsize.empty())
            s += opsize + " ";

        s += opstr;
    }

    return s;
}

std::string Printer::reg(const RegisterOperand &regop) const { return "$" + std::to_string(regop.r); }

std::string Printer::disp(const Operand *operand) const
{
    std::string s;

    if(operand->disp.base.isValid())
        s += this->reg(operand->disp.base);

    if(operand->disp.index.isValid())
    {
        if(!s.empty())
            s += " + ";

        s += this->reg(operand->disp.index);

        if(operand->disp.scale > 1)
            s += " * " + REDasm::hex(operand->disp.scale);
    }

    if(operand->disp.displacement)
    {
        if(operand->disp.displacement > 0)
        {
            Symbol* symbol = m_document->symbol(operand->disp.displacement);

            if(symbol)
                s += " + " + symbol->name;
            else
                s += " + " + REDasm::hex(operand->disp.displacement);
        }
        else if(operand->disp.displacement < 0)
            s += " - " + REDasm::hex(std::abs(operand->disp.displacement));
    }

    if(operand->is(OperandTypes::Local) || operand->is(OperandTypes::Argument))
    {
        std::string loc = this->loc(operand);

        if(!loc.empty())
        {
            if(!s.empty())
                s += " + ";

            s += loc;
        }
    }

    return "[" + s + "]";
}

std::string Printer::loc(const Operand *operand) const
{
    RE_UNUSED(operand);
    return std::string();
}

std::string Printer::mem(const Operand *operand) const { return this->imm(operand); }

std::string Printer::imm(const Operand *operand) const
{
    Symbol* symbol = m_disassembler->document()->symbol(operand->u_value);

    if(operand->is(OperandTypes::Memory))
        return "[" + (symbol ? symbol->name : REDasm::hex(operand->u_value)) + "]";

    return symbol ? symbol->name : REDasm::hex(operand->s_value);
}

std::string Printer::size(const Operand *operand) const { return OperandSizes::size(operand->size); }

CapstonePrinter::CapstonePrinter(csh cshandle, DisassemblerAPI *disassembler): Printer(disassembler), m_cshandle(cshandle) { }

std::string CapstonePrinter::reg(const RegisterOperand& regop) const
{
    if(regop.r <= 0)
    {
        REDasm::log("Unknown register with id " + std::to_string(regop.r));
        return "unkreg";
    }

    return cs_reg_name(m_cshandle, static_cast<unsigned int>(regop.r));
}

} // namespace REDasm
