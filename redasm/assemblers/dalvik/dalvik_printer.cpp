#include "dalvik_printer.h"
#include "dalvik_metadata.h"
#include "../../loaders/dex/dex.h"
#include "../../loaders/dex/dex_constants.h"

namespace REDasm {

DalvikPrinter::DalvikPrinter(DisassemblerAPI *disassembler): Printer(disassembler)
{

}

void DalvikPrinter::function(const Symbol *symbol, const Printer::FunctionCallback& headerfunc)
{
    auto* dexloader = dynamic_cast<DEXLoader*>(m_disassembler->loader());

    if(!dexloader)
    {
        Printer::function(symbol, headerfunc);
        return;
    }

    // Reset printer data
    m_regnames.clear();
    m_regoverrides.clear();
    m_currentdbginfo.line_start = DEX_NO_INDEX_U;

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

void DalvikPrinter::prologue(const Symbol* symbol, const LineCallback &prologuefunc)
{
    auto* dexloader = dynamic_cast<DEXLoader*>(m_disassembler->loader());

    if(!dexloader)
        return;

    DEXEncodedMethod dexmethod;

    if(!dexloader->getMethodInfo(symbol->tag, dexmethod))
        return;

    if(!dexloader->getDebugInfo(symbol->tag, m_currentdbginfo))
        return;

    u32 delta = (dexmethod.access_flags & DexAccessFlags::Static) ? 0 : 1;

    for(auto it = m_currentdbginfo.parameter_names.begin(); it != m_currentdbginfo.parameter_names.end(); it++)
    {
        u64 argidx = std::distance(m_currentdbginfo.parameter_names.begin(), it) + delta;
        prologuefunc(".arg" + std::to_string(argidx) + ": " + *it);
    }
}

void DalvikPrinter::info(const InstructionPtr &instruction, const LineCallback &infofunc)
{
    if(m_currentdbginfo.line_start == DEX_NO_INDEX_U)
        return;

    auto dbgit = m_currentdbginfo.debug_data.find(static_cast<u16>(instruction->address));

    if(dbgit == m_currentdbginfo.debug_data.end())
        return;

    DEXLoader* dexloader = dynamic_cast<DEXLoader*>(m_disassembler->loader());

    if(!dexloader)
        return;

    for(auto it = dbgit->second.begin(); it != dbgit->second.end(); it++)
    {
        const DEXDebugData& debugdata = *it;

        if((debugdata.data_type == DEXDebugDataTypes::StartLocal) || ((debugdata.data_type == DEXDebugDataTypes::StartLocalExtended)))
        {
            if(debugdata.name_idx == DEX_NO_INDEX)
                continue;

            this->startLocal(dexloader, debugdata);
            const std::string& name = dexloader->getString(debugdata.name_idx);
            std::string type;

            if(debugdata.type_idx != DEX_NO_INDEX)
                type = ": " + dexloader->getType(debugdata.type_idx);

            infofunc(".local " + DalvikPrinter::registerName(debugdata.register_num) + " = " + name + type);
        }
        else if(debugdata.data_type == DEXDebugDataTypes::RestartLocal)
            this->restoreLocal(dexloader, debugdata.register_num);
        else if(debugdata.data_type == DEXDebugDataTypes::EndLocal)
            this->endLocal(debugdata.register_num);
        else if(debugdata.data_type == DEXDebugDataTypes::PrologueEnd)
            infofunc(".prologue_end");
        else if(debugdata.data_type == DEXDebugDataTypes::Line)
            infofunc(".line " + std::to_string(debugdata.line_no));
    }
}

std::string DalvikPrinter::reg(const RegisterOperand &regop) const
{
    std::string s;

    auto it = m_regnames.find(regop.r);

    if(it != m_regnames.end())
        s = it->second;
    else
        s = DalvikPrinter::registerName(regop.r);

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

std::string DalvikPrinter::registerName(register_id_t r) { return "v" + std::to_string(r); }

void DalvikPrinter::startLocal(DEXLoader* dexloader, const DEXDebugData &debugdata)
{
    m_regoverrides[debugdata.register_num] = debugdata;
    m_regnames[debugdata.register_num] = dexloader->getString(debugdata.name_idx);
}

void DalvikPrinter::restoreLocal(DEXLoader* dexloader, register_id_t r)
{
    auto it = m_regoverrides.find(r);

    if(it == m_regoverrides.end())
        return;

    const DEXDebugData& debugdata = it->second;
    m_regnames[r] = dexloader->getString(debugdata.name_idx);
}

void DalvikPrinter::endLocal(register_id_t r)
{
    auto it = m_regnames.find(r);

    if(it != m_regnames.end())
        m_regnames.erase(it);
}

} // namespace REDasm
