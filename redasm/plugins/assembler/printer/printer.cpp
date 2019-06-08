#include "printer.h"
#include <impl/plugins/assembler/printer/printer_impl.h>
#include "../../../disassembler/disassembler.h"
#include "../../../support/utils.h"
#include "../../loader/loader.h"
#include "../assembler.h"
#include <cmath>

#define HEADER_SYMBOL_COUNT 10

namespace REDasm {

Printer::Printer(PrinterImpl *p): m_pimpl_p(p) { }
Printer::Printer(Disassembler *disassembler): m_pimpl_p(new PrinterImpl(disassembler)) { }
Disassembler *Printer::disassembler() const { PIMPL_P(const Printer); return p->m_disassembler; }
const ListingDocument &Printer::document() const { PIMPL_P(const Printer); return p->m_document; }

std::string Printer::symbol(const Symbol* symbol) const
{
    if(symbol->is(SymbolType::Pointer))
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
    u32 bits = this->disassembler()->assembler()->bits();

    segmentfunc(s + " SEGMENT " + (segment ? Utils::quoted(segment->name) : "???") +
                    " START: " + Utils::hex(segment->address, bits) +
                    " END: " + Utils::hex(segment->endaddress, bits) + " " + s);
}

void Printer::function(const Symbol* symbol, const Printer::FunctionCallback& functionfunc)
{
    std::string s(HEADER_SYMBOL_COUNT, '=');
    functionfunc(s + " FUNCTION ", symbol->name, " " + s);
}

void Printer::symbol(const Symbol* symbol, const SymbolCallback &symbolfunc) const
{
    if(symbol->isFunction() || symbol->is(SymbolType::Code))
        return;

    const Segment* segment = this->disassembler()->document()->segment(symbol->address);

    if(!segment)
        return;

    if(symbol->is(SymbolType::Pointer))
    {
        const Symbol* ptrsymbol = this->disassembler()->dereferenceSymbol(symbol);

        if(ptrsymbol)
        {
            symbolfunc(symbol, ptrsymbol->name);
            this->symbol(ptrsymbol, symbolfunc); // Emit pointed symbol too
            return;
        }
    }

    if(symbol->is(SymbolType::Data))
    {
        if(segment->is(SegmentType::Bss))
        {
            symbolfunc(symbol, "??");
            return;
        }

        Assembler* assembler = this->disassembler()->assembler();
        u64 value = 0;

        if(!this->disassembler()->readAddress(symbol->address, assembler->addressWidth(), &value))
            return;

        symbolfunc(symbol, Utils::hex(value, assembler->addressWidth()));
    }
    else if(symbol->is(SymbolType::WideStringMask))
        symbolfunc(symbol, " \"" + this->disassembler()->readWString(symbol->address) + "\"");
    else if(symbol->is(SymbolType::String))
        symbolfunc(symbol, " \"" + this->disassembler()->readString(symbol->address) + "\"");
}

std::string Printer::out(const InstructionPtr &instruction, const OpCallback &opfunc) const
{
    const OperandList& operands = instruction->operands;
    std::string s = instruction->mnemonic;

    if(instruction->isInvalid())
    {
        BufferView view = this->disassembler()->loader()->view(instruction->address);
        std::string hexstring = Utils::hexstring(&view, instruction->size);

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
        const Operand& op = *it;

        if(op.is(OperandType::Constant))
            opstr = Utils::hex(op.u_value, 0, true);
        else if(op.is(OperandType::Immediate))
            opstr = this->imm(&op);
        else if(op.is(OperandType::Memory))
            opstr = this->mem(&op);
        else if(op.is(OperandType::Displacement))
            opstr = this->disp(&op);
        else if(op.is(OperandType::Register))
            opstr = this->reg(op.reg);
        else
            continue;

        std::string opsize = this->size(&op);

        if(opfunc)
            opfunc(&op, opsize, opstr);

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

    if(operand->is(OperandType::Local) || operand->is(OperandType::Argument))
    {
        std::string loc = this->loc(operand);

        if(!loc.empty())
        {
            if(!s.empty())
                s += "+";

            s += loc;
            return "[" + s + "]";
        }
    }

    if(operand->disp.index.isValid())
    {
        if(!s.empty())
            s += "+";

        s += this->reg(operand->disp.index);

        if(operand->disp.scale > 1)
            s += "*" + Utils::hex(operand->disp.scale);
    }

    if(operand->disp.displacement)
    {
        if(operand->disp.displacement > 0)
        {
            Symbol* symbol = this->document()->symbol(operand->disp.displacement);

            if(symbol)
                s += "+" + symbol->name;
            else
                s += "+" + Utils::hex(operand->disp.displacement);
        }
        else if(operand->disp.displacement < 0)
            s += "-" + Utils::hex(std::abs(operand->disp.displacement));
    }

    return "[" + s + "]";
}

std::string Printer::loc(const Operand *operand) const { return std::string(); }
std::string Printer::mem(const Operand *operand) const { return this->imm(operand); }

std::string Printer::imm(const Operand *operand) const
{
    Symbol* symbol = this->disassembler()->document()->symbol(operand->u_value);

    if(operand->is(OperandType::Memory))
        return "[" + (symbol ? symbol->name : Utils::hex(operand->u_value)) + "]";

    return symbol ? symbol->name : Utils::hex(operand->s_value);
}

std::string Printer::size(const Operand *operand) const { return std::string(); }

} // namespace REDasm
