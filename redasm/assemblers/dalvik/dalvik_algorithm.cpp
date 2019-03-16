#include "dalvik_algorithm.h"
#include "../../loaders/dex/dex.h"
#include "../../assemblers/dalvik/dalvik.h"
#include "../../redasm/support/symbolize.h"
#include "dalvik_payload.h"

namespace REDasm {

DalvikAlgorithm::DalvikAlgorithm(DisassemblerAPI* disassembler, AssemblerPlugin* assemblerplugin): AssemblerAlgorithm(disassembler, assemblerplugin)
{
    m_dexloader = dynamic_cast<DEXLoader*>(disassembler->loader());

    REGISTER_STATE(DalvikAlgorithm::StringIndexState, &DalvikAlgorithm::stringIndexState);
    REGISTER_STATE(DalvikAlgorithm::MethodIndexState, &DalvikAlgorithm::methodIndexState);
    REGISTER_STATE(DalvikAlgorithm::PackedSwitchTableState, &DalvikAlgorithm::packedSwitchTableState);
    REGISTER_STATE(DalvikAlgorithm::SparseSwitchTableState, &DalvikAlgorithm::sparseSwitchTableState);
    REGISTER_STATE(DalvikAlgorithm::FillArrayDataState, &DalvikAlgorithm::fillArrayDataState);
    REGISTER_STATE(DalvikAlgorithm::DebugInfoState, &DalvikAlgorithm::debugInfoState);
}

void DalvikAlgorithm::validateTarget(const InstructionPtr &) const { /* Nop */ }
void DalvikAlgorithm::onDecodeFailed(const InstructionPtr &instruction) { REDasm::log("Found invalid instruction @ " + REDasm::hex(instruction->address)); }

void DalvikAlgorithm::onDecodedOperand(const Operand* op, const InstructionPtr &instruction)
{
    if(op->tag == DalvikOperands::StringIndex)
        EXECUTE_STATE(DalvikAlgorithm::StringIndexState, op->tag, op->index, instruction);
    else if(op->tag == DalvikOperands::MethodIndex)
        EXECUTE_STATE(DalvikAlgorithm::MethodIndexState, op->tag, op->index, instruction);
    else if(op->tag == DalvikOperands::PackedSwitchTable)
        EXECUTE_STATE(DalvikAlgorithm::PackedSwitchTableState, op->tag, op->index, instruction);
    else if(op->tag == DalvikOperands::SparseSwitchTable)
        EXECUTE_STATE(DalvikAlgorithm::SparseSwitchTableState, op->tag, op->index, instruction);
    else if(op->tag == DalvikOperands::FillArrayData)
        EXECUTE_STATE(DalvikAlgorithm::FillArrayDataState, op->tag, op->index, instruction);
}

void DalvikAlgorithm::onDecoded(const InstructionPtr &instruction)
{
    AssemblerAlgorithm::onDecoded(instruction);

    auto it = m_methodbounds.find(instruction->endAddress());

    if((it == m_methodbounds.end()) && this->canContinue(instruction))
        this->enqueue(instruction->endAddress());
    else if(it != m_methodbounds.end())
        m_methodbounds.erase(it);
}

void DalvikAlgorithm::decodeState(const State *state)
{
    Symbol* symbol = m_document->symbol(state->address);

    if(symbol && symbol->isFunction())
    {
        m_methodbounds.insert(state->address + m_dexloader->getMethodSize(symbol->tag));
        FORWARD_STATE(DalvikAlgorithm::DebugInfoState, state);
    }

    AssemblerAlgorithm::decodeState(state);
}

void DalvikAlgorithm::stringIndexState(const State *state)
{
    if(!m_dexloader)
        return;

    const Operand* op = state->operand();
    offset_t offset = 0;

    if(!m_dexloader->getStringOffset(op->u_value, offset))
        return;

    m_document->symbol(offset, SymbolTypes::String);
    m_disassembler->pushReference(offset, state->instruction->address);
}

void DalvikAlgorithm::methodIndexState(const State *state)
{
    if(!m_dexloader)
        return;

    this->checkImport(state);

    const Operand* op = state->operand();
    offset_t offset = 0;

    if(!m_dexloader->getMethodOffset(op->u_value, offset))
        return;

    m_disassembler->pushReference(offset, state->instruction->address);
}

void DalvikAlgorithm::packedSwitchTableState(const State *state)
{
    const Operand* op = state->operand();
    REDasm::symbolize<DalvikPackedSwitchPayload>(m_disassembler, op->u_value, "packed_switch");
    DalvikPackedSwitchPayload* packedswitchpayload = m_loader->addrpointer<DalvikPackedSwitchPayload>(op->u_value);

    if(!packedswitchpayload)
        return;

    InstructionPtr instruction = state->instruction;
    m_document->autoComment(instruction->address, std::to_string(packedswitchpayload->size) + " case(s)");
    u32* targets = packedswitchpayload->targets;
    CaseMap cases;

    for(u16 i = 0; i < packedswitchpayload->size; i++, targets++)
    {
        u32 caseidx = packedswitchpayload->first_key + i;
        address_t address = instruction->address + (*targets * sizeof(u16));
        this->enqueue(address);

        m_document->lock(m_loader->addressof(targets), "packed_switch_" + REDasm::hex(op->u_value) + "_case_" + std::to_string(caseidx), SymbolTypes::Pointer | SymbolTypes::Data);
        m_document->symbol(address, SymbolTypes::Code);
        m_disassembler->pushReference(address, instruction->address);
        instruction->target(address);

        auto it = cases.find(address);

        if(it != cases.end())
            it->second.push_back(caseidx);
        else
            cases[address] = { caseidx };
    }

    this->emitCaseInfo(op->u_value, cases);
    m_document->update(instruction);
}

void DalvikAlgorithm::sparseSwitchTableState(const State *state)
{

}

void DalvikAlgorithm::fillArrayDataState(const State *state)
{

}

void DalvikAlgorithm::debugInfoState(const State *state)
{
    Symbol* symbol = m_document->symbol(state->address);

    if(!symbol || !symbol->isFunction())
        return;

    DEXEncodedMethod dexmethod;

    if(!m_dexloader->getMethodInfo(symbol->tag, dexmethod))
        return;

    DEXDebugInfo dexdebuginfo;

    if(!m_dexloader->getDebugInfo(symbol->tag, dexdebuginfo))
        return;

    this->emitArguments(state, dexmethod, dexdebuginfo);
    this->emitDebugData(dexdebuginfo);
}

void DalvikAlgorithm::emitCaseInfo(address_t address, const DalvikAlgorithm::CaseMap &casemap)
{
    for(const auto& item : casemap)
    {
        std::string casestring;

        std::for_each(item.second.begin(), item.second.end(), [&casestring](u32 caseidx) {
            if(!casestring.empty())
                casestring += ", ";

            casestring += "#" + std::to_string(caseidx);
        });

        m_document->meta(item.first, "Packaged Switch Table @ " + REDasm::hex(address) + " (Case(s) " + casestring + ")");
    }
}

void DalvikAlgorithm::emitArguments(const State* state, const DEXEncodedMethod& dexmethod, const DEXDebugInfo& dexdebuginfo)
{
    u32 delta = (dexmethod.access_flags & DexAccessFlags::Static) ? 0 : 1;

    for(size_t i = 0; i < dexdebuginfo.parameter_names.size(); i++)
    {
        const std::string& param = dexdebuginfo.parameter_names[i];
        m_document->meta(state->address, "arg_" + std::to_string(i + delta) + ": " + param);
    }
}

void DalvikAlgorithm::emitDebugData(const DEXDebugInfo &dexdebuginfo)
{
    if(dexdebuginfo.line_start == DEX_NO_INDEX_U)
        return;

    for(const auto& item: dexdebuginfo.debug_data)
    {
        for(const auto& dbgdata : item.second)
        {
            if((dbgdata.data_type == DEXDebugDataTypes::StartLocal) || ((dbgdata.data_type == DEXDebugDataTypes::StartLocalExtended)))
            {
                if(dbgdata.name_idx == DEX_NO_INDEX)
                    continue;

                const std::string& name = m_dexloader->getString(dbgdata.name_idx);
                std::string type;

                if(dbgdata.type_idx != DEX_NO_INDEX)
                    type = ": " + m_dexloader->getType(dbgdata.type_idx);

                m_document->meta(item.first, "localstart " + DalvikAssembler::registerName(dbgdata.register_num) + " = " + name + type);
            }
            else if(dbgdata.data_type == DEXDebugDataTypes::RestartLocal)
                m_document->meta(item.first, "localrestart " + DalvikAssembler::registerName(dbgdata.register_num));
            else if(dbgdata.data_type == DEXDebugDataTypes::EndLocal)
                m_document->meta(item.first, "localend " + DalvikAssembler::registerName(dbgdata.register_num));
            else if(dbgdata.data_type == DEXDebugDataTypes::PrologueEnd)
                m_document->meta(item.first, "prologue_end");
            else if(dbgdata.data_type == DEXDebugDataTypes::Line)
                m_document->meta(item.first, "line " + std::to_string(dbgdata.line_no));
        }
    }
}

void DalvikAlgorithm::checkImport(const State* state)
{
    const Operand* op = state->operand();
    const std::string& methodname = m_dexloader->getMethodName(op->u_value);

    auto it = m_imports.find(methodname);

    if(it != m_imports.end())
        return;

    m_imports.insert(methodname);
    address_t importaddress = 0;

    if(!methodname.find("java."))
        m_document->symbol(m_dexloader->nextImport(&importaddress), methodname, SymbolTypes::Import);
    else
        return;

    m_disassembler->pushReference(importaddress, state->instruction->address);
}

bool DalvikAlgorithm::canContinue(const InstructionPtr &instruction)
{
    if(instruction->is(InstructionTypes::Stop))
        return false;

    if(instruction->is(InstructionTypes::Jump) && !instruction->is(InstructionTypes::Conditional))
        return false;

    return true;
}

} // namespace REDasm
