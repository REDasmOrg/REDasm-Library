#include "printer.h"
#include "../../../disassembler/disassembler.h"
#include "../../../support/utils.h"
#include "../../../context.h"
#include "../../loader/loader.h"
#include "../assembler.h"
#include <cmath>

#define HEADER_SYMBOL_COUNT 10

namespace REDasm {

String Printer::symbol(const Symbol* symbol) const
{
    //if(symbol->is(Symbol::Pointer))
        //return symbol->name;

    String s;

    this->symbol(symbol, [&s](const Symbol*, String line) {
        s = line;
    });

    return s;
}

String Printer::out(const CachedInstruction &instruction) const
{
    return this->out(instruction, [](const Operand*, const String&, const String&) { });
}

void Printer::segment(const Segment *segment, const Printer::LineCallback& segmentfunc)
{
    String s = String::repeated('=', HEADER_SYMBOL_COUNT * 2);

    segmentfunc(s + " SEGMENT " + (segment ? segment->name().quoted() : "???") +
                    " START: " + String::hex(segment->address, r_asm->bits()) +
                    " END: " + String::hex(segment->endaddress, r_asm->bits()) + " " + s);
}

void Printer::function(const Symbol* symbol, const Printer::FunctionCallback& functionfunc)
{
    String s = String::repeated('=', HEADER_SYMBOL_COUNT);
    functionfunc(s + " FUNCTION ", symbol->name, " " + s);
}

void Printer::symbol(const Symbol* symbol, const SymbolCallback &symbolfunc) const
{
    if(symbol->isFunction() || symbol->isLabel()) return;

    const Segment* segment = r_doc->segment(symbol->address);
    if(!segment) return;

    // if(symbol->is(Symbol::Pointer))
    // {
    //     const Symbol* ptrsymbol = r_disasm->dereferenceSymbol(symbol);

    //     if(ptrsymbol)
    //     {
    //         symbolfunc(symbol, ptrsymbol->name);
    //         this->symbol(ptrsymbol, symbolfunc); // Emit pointed symbol too
    //         return;
    //     }
    // }

    if(symbol->isData())
    {
        if(segment->is(Segment::T_Bss))
        {
            symbolfunc(symbol, "??");
            return;
        }

        u64 value = 0;

        if(!r_disasm->readAddress(symbol->address, r_asm->addressWidth(), &value))
            return;

        symbolfunc(symbol, String::hex(value, r_asm->addressWidth()));
    }
    else if(symbol->isString())
    {
        if(symbol->isWideString()) symbolfunc(symbol, " \"" + r_disasm->readWString(symbol->address) + "\"");
        else symbolfunc(symbol, " \"" + r_disasm->readString(symbol->address) + "\"");
    }
}

String Printer::out(const CachedInstruction &instruction, const OpCallback &opfunc) const
{
    String s = instruction->mnemonic();

    if(instruction->isInvalid())
    {
        BufferView view = r_ldr->view(instruction->address);
        String hexstring = String::hexstring(view.data(), instruction->size);

        s += hexstring;
        opfunc(nullptr, String(), hexstring);
        return s;
    }

    if(instruction->hasOperands()) s += " ";

    for(size_t i = 0; i < instruction->operandscount; i++)
    {
        if(i) s += ", ";

        String opstr;
        const Operand* op = instruction->op(i);

        switch(op->type)
        {
            case Operand::T_Constant:     opstr = String::hex(op->u_value, 0, true); break;
            case Operand::T_Immediate:    opstr = this->imm(op); break;
            case Operand::T_Memory:       opstr = this->mem(op); break;
            case Operand::T_Displacement: opstr = this->disp(op); break;
            case Operand::T_Register:     opstr = this->reg(&op->reg); break;
            default: continue;
        }

        String opsize = this->size(op);
        if(opfunc) opfunc(op, opsize, opstr);
        if(!opsize.empty()) s += opsize + " ";
        s += opstr;
    }

    return s;
}

String Printer::reg(const RegisterOperand* regop) const { return "$" + String::number(regop->r); }

String Printer::disp(const Operand* op) const
{
    String s;
    if(op->isBaseValid()) s += this->reg(&op->disp.basestruct);

    if(hasFlag(op, Operand::F_Local) || hasFlag(op, Operand::F_Argument))
    {
        String loc = this->loc(op);

        if(!loc.empty())
        {
            if(!s.empty())
                s += "+";

            s += loc;
            return "[" + s + "]";
        }
    }

    if(op->isIndexValid())
    {
        if(!s.empty()) s += "+";
        s += this->reg(&op->disp.indexstruct);

        if(op->disp.scale > 1)
            s += "*" + String::hex(op->disp.scale);
    }

    if(op->disp.displacement)
    {
        if(op->disp.displacement > 0)
        {
            const Symbol* symbol = r_doc->symbol(op->disp.displacement);
            if(symbol) s += "+" + symbol->name;
            else s += "+" + String::hex(op->disp.displacement);
        }
        else if(op->disp.displacement < 0)
            s += "-" + String::hex(std::abs(op->disp.displacement));
    }

    return "[" + s + "]";
}

String Printer::loc(const Operand* op) const { return String(); }
String Printer::mem(const Operand* op) const { return this->imm(op); }

String Printer::imm(const Operand* op) const
{
    const Symbol* symbol = r_doc->symbol(op->u_value);

    if(REDasm::typeIs(op, Operand::T_Memory))
        return "[" + (symbol ? symbol->name : String::hex(op->u_value)) + "]";

    return symbol ? symbol->name : String::hex(op->s_value);
}

String Printer::size(const Operand* op) const { return String(); }

} // namespace REDasm
