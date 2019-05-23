#include "dalvik_printer.h"
#include "dalvik_metadata.h"
#include "../../loaders/dex/dex.h"
#include "../../assemblers/dalvik/dalvik.h"
#include "../../loaders/dex/dex_constants.h"

namespace REDasm {

DalvikPrinter::DalvikPrinter(DisassemblerAPI *disassembler): Printer(disassembler) { }

void DalvikPrinter::function(const Symbol *symbol, const Printer::FunctionCallback& headerfunc)
{
    auto* dexloader = dynamic_cast<DEXLoader*>(m_disassembler->loader());

    if(!dexloader)
    {
        Printer::function(symbol, headerfunc);
        return;
    }

    DEXEncodedMethod dexmethod;
    std::string access;

    if(dexloader->getMethodInfo(symbol->tag, dexmethod))
    {
        if(dexmethod.access_flags & DexAccessFlags::Public)
            access += access.empty() ? "public" : " public";

        if(dexmethod.access_flags & DexAccessFlags::Protected)
            access += access.empty() ? "protected" : " protected";

        if(dexmethod.access_flags & DexAccessFlags::Private)
            access += access.empty() ? "private" : " private";

        if(dexmethod.access_flags & DexAccessFlags::Static)
            access += access.empty() ? "static" : " static";

        if(!access.empty())
            access += " ";
    }

    headerfunc(access + dexloader->getReturnType(symbol->tag) + " ",
               symbol->name, dexloader->getParameters(symbol->tag));
}

std::string DalvikPrinter::reg(const RegisterOperand &regop) const
{
    std::string s = DalvikAssembler::registerName(regop.r);

    if(regop.tag & DalvikOperands::ParameterFirst)
        s = "{" + s;

    if(regop.tag & DalvikOperands::ParameterLast)
        s += "}";

    return s;
}

std::string DalvikPrinter::imm(const Operand *op) const
{
    DEXLoader* dexloader = nullptr;

    if(op->tag && (dexloader = dynamic_cast<DEXLoader*>(m_disassembler->loader())))
    {
        switch(op->tag)
        {
            case DalvikOperands::StringIndex:
                return REDasm::quoted(dexloader->getString(op->u_value));

            case DalvikOperands::TypeIndex:
                return dexloader->getType(op->u_value);

            case DalvikOperands::MethodIndex:
                return dexloader->getMethodProto(op->u_value);

            case DalvikOperands::FieldIndex:
                return dexloader->getField(op->u_value);

            default:
                break;
        }
    }

    return Printer::imm(op);
}

} // namespace REDasm
