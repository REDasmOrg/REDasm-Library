#include "dalvik_algorithm.h"
#include "../../formats/dex/dex.h"

namespace REDasm {

DalvikAlgorithm::DalvikAlgorithm(DisassemblerAPI* disassembler, AssemblerPlugin* assemblerplugin): AssemblerAlgorithm(disassembler, assemblerplugin)
{
    m_dexformat = dynamic_cast<DEXFormat*>(disassembler->format());

    REGISTER_STATE(DalvikAlgorithm::StringIndexState, &DalvikAlgorithm::stringIndexState);
    REGISTER_STATE(DalvikAlgorithm::MethodIndexState, &DalvikAlgorithm::methodIndexState);
}

void DalvikAlgorithm::onDecodeFailed(const InstructionPtr &instruction) { REDasm::log("Found invalid instruction @ " + REDasm::hex(instruction->address)); }

void DalvikAlgorithm::onDecodedOperand(const Operand& op, const InstructionPtr &instruction)
{
    if(op.extra_type == DalvikOperands::StringIndex)
        ENQUEUE_STATE(DalvikAlgorithm::StringIndexState, op.extra_type, op.index, instruction);
    else if(op.extra_type == DalvikOperands::MethodIndex)
        ENQUEUE_STATE(DalvikAlgorithm::MethodIndexState, op.extra_type, op.index, instruction);
}

void DalvikAlgorithm::onDecoded(const InstructionPtr &instruction)
{
    AssemblerAlgorithm::onDecoded(instruction);

    auto it = m_methodbounds.find(instruction->endAddress());

    if(it == m_methodbounds.end())
        this->enqueue(instruction->endAddress());
    else
        m_methodbounds.erase(it);
}

void DalvikAlgorithm::decodeState(State *state)
{
    SymbolPtr symbol = m_document->symbol(state->address);

    if(symbol && symbol->isFunction())
        m_methodbounds.insert(state->address + m_dexformat->getMethodSize(symbol->tag));

    AssemblerAlgorithm::decodeState(state);
}

void DalvikAlgorithm::stringIndexState(const State *state)
{
    if(!m_dexformat)
        return;

    const Operand& op = state->operand();
    offset_t offset = 0;

    if(!m_dexformat->getStringOffset(op.u_value, offset))
        return;

    m_document->symbol(offset, SymbolTypes::String);
    m_disassembler->pushReference(offset, state->instruction->address);
}

void DalvikAlgorithm::methodIndexState(const State *state)
{
    if(!m_dexformat)
        return;

    this->checkImport(state);

    const Operand& op = state->operand();
    offset_t offset = 0;

    if(!m_dexformat->getMethodOffset(op.u_value, offset))
        return;

    m_disassembler->pushReference(offset, state->instruction->address);
}

void DalvikAlgorithm::checkImport(const State* state)
{
    const Operand& op = state->operand();
    const std::string& methodname = m_dexformat->getMethodName(op.u_value);

    auto it = m_imports.find(methodname);

    if(it != m_imports.end())
        return;

    m_imports.insert(methodname);
    address_t importaddress = 0;

    if(!methodname.find("java."))
        m_document->symbol(m_dexformat->nextImport(&importaddress), methodname, SymbolTypes::Import);
    else
        return;

    m_disassembler->pushReference(importaddress, state->instruction->address);
}

} // namespace REDasm
