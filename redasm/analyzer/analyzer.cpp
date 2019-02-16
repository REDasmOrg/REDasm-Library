#include "analyzer.h"
#include "../support/hash.h"

namespace REDasm {

Analyzer::Analyzer(DisassemblerAPI *disassembler, const SignatureFiles &signaturefiles): m_document(disassembler->document()), m_disassembler(disassembler), m_signaturefiles(signaturefiles) { }
Analyzer::~Analyzer() { }
void Analyzer::analyzeFast()  { this->checkFunctions(); }

void Analyzer::analyze()
{
    this->loadSignatures();
    this->checkFunctions();
}

void Analyzer::checkFunctions()
{
    m_disassembler->document()->symbols()->iterate(SymbolTypes::FunctionMask, [this](SymbolPtr symbol) -> bool {
        if(!this->findNullSubs(symbol))
            this->findTrampoline(symbol);

        return true;
    });
}

void Analyzer::loadSignatures()
{
    for(const std::string& sdbfile : m_signaturefiles)
        m_disassembler->loadSignature(REDasm::makeSdbPath(sdbfile));
}

bool Analyzer::findNullSubs(const SymbolPtr& symbol)
{
    auto it = m_document->instructionItem(symbol->address);

    if(it == m_document->end())
        return true; // Don't execute trampoline analysis

    InstructionPtr instruction = m_document->instruction((*it)->address);

    if(!instruction)
        return true; // Don't execute trampoline analysis

    if(!instruction->is(InstructionTypes::Stop))
        return false;

    m_document->lock(symbol->address, "nullsub_" + REDasm::hex(symbol->address));
    return true;
}

void Analyzer::findTrampoline(const SymbolPtr& symbol)
{
    auto it = m_document->instructionItem(symbol->address);

    if(it == m_document->end())
        return;

    const AssemblerPlugin* assembler = m_disassembler->assembler();
    SymbolPtr symtrampoline;

    if(ASSEMBLER_IS(assembler, "x86"))
        symtrampoline = this->findTrampoline_x86(it);
    else if(ASSEMBLER_IS(assembler, "ARM"))
        symtrampoline = this->findTrampoline_arm(it);

    if(!symtrampoline)
        return;

    SymbolPtr symentry = m_document->documentEntry();

    if(!symtrampoline->is(SymbolTypes::Import))
    {
        m_document->function(symtrampoline->address);

        if(!symbol->isLocked())
        {
            symtrampoline = m_document->symbol(symtrampoline->address); // Get updated symbol name from cache
            m_document->rename(symbol->address, REDasm::trampoline(symtrampoline->name, "jmp_to"));
        }
        else if(symbol->address == symentry->address)
        {
            m_document->rename(symtrampoline->address, START_FUNCTION);
            m_document->setDocumentEntry(symtrampoline->address);
        }
    }
    else if(symbol->address != symentry->address)
        m_document->lock(symbol->address, REDasm::trampoline(symtrampoline->name));

    InstructionPtr instruction = m_document->instruction(symbol->address);

    if(!instruction)
        return;

    m_disassembler->pushReference(symtrampoline->address, instruction->address);
}

SymbolPtr Analyzer::findTrampoline_x86(ListingDocumentType::iterator it)
{
    InstructionPtr instruction = m_disassembler->document()->instruction((*it)->address);

    if(!instruction->is(InstructionTypes::Jump) || !instruction->hasTargets())
        return NULL;

    return m_disassembler->document()->symbol(instruction->target());
}

SymbolPtr Analyzer::findTrampoline_arm(ListingDocumentType::iterator it)
{
    auto& doc = m_disassembler->document();
    InstructionPtr instruction1 = doc->instruction((*it)->address);
    it++;

    if(it == doc->end() || !(*it)->is(ListingItem::InstructionItem))
        return NULL;

    const InstructionPtr& instruction2 = doc->instruction((*it)->address);

    if(!instruction1 || !instruction2 || instruction1->isInvalid() || instruction2->isInvalid())
        return NULL;

    if((instruction1->mnemonic != "ldr") && (instruction2->mnemonic != "ldr"))
        return NULL;

    if(!instruction1->op(1)->is(OperandTypes::Memory) || (instruction2->op(0)->reg.r != ARM_REG_PC))
        return NULL;

    u64 target = instruction1->op(1)->u_value, importaddress = 0;

    if(!m_disassembler->readAddress(target, sizeof(u32), &importaddress))
        return NULL;

    SymbolPtr symbol = doc->symbol(target), impsymbol = doc->symbol(importaddress);

    if(symbol && impsymbol)
        doc->lock(symbol->address, "imp." + impsymbol->name);

    return impsymbol;
}

} // namespace REDasm
