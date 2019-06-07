#include "analyzer.h"
#include <impl/plugins/assembler/analyzer_impl.h>

namespace REDasm {

Analyzer::Analyzer(Disassembler *disassembler): m_pimpl_p(new AnalyzerImpl(disassembler)) { }
void Analyzer::analyzeFast() { PIMPL_P(Analyzer); p->analyzeFast(); }
void Analyzer::analyze() { this->analyzeFast(); }

/*
void Analyzer::findTrampoline(const Symbol* symbol)
{
}

Symbol* Analyzer::findTrampoline_x86(ListingDocumentType::const_iterator& it)
{
    InstructionPtr instruction = m_disassembler->document()->instruction((*it)->address);

    if(!instruction->is(InstructionType::Jump))
        return nullptr;

    auto target = m_disassembler->getTarget((*it)->address);

    if(!target.valid)
        return nullptr;

    return m_disassembler->document()->symbol(target);
}

Symbol* Analyzer::findTrampoline_arm(ListingDocumentType::const_iterator &it)
{
    auto& doc = m_disassembler->document();
    InstructionPtr instruction1 = doc->instruction((*it)->address);
    it++;

    if(it == doc->end() || !(*it)->is(ListingItem::InstructionItem))
        return nullptr;

    const InstructionPtr& instruction2 = doc->instruction((*it)->address);

    if(!instruction1 || !instruction2 || instruction1->isInvalid() || instruction2->isInvalid())
        return nullptr;

    if((instruction1->mnemonic != "ldr") || (instruction2->mnemonic != "ldr"))
        return nullptr;

    if(!instruction1->op(1)->is(OperandType::Memory) || (instruction2->op(0)->reg.r != ARM_REG_PC))
        return nullptr;

    u64 target = instruction1->op(1)->u_value, importaddress = 0;

    if(!m_disassembler->readAddress(target, sizeof(u32), &importaddress))
        return nullptr;

    Symbol *symbol = doc->symbol(target), *impsymbol = doc->symbol(importaddress);

    if(symbol && impsymbol)
        doc->lock(symbol->address, "imp." + impsymbol->name);

    return impsymbol;
}
*/

} // namespace REDasm
