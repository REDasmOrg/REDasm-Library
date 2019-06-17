#include "analyzer_impl.h"
#include <redasm/disassembler/listing/listingdocumentiterator.h>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/plugins/loader/loader.h>
#include <redasm/support/utils.h>

namespace REDasm {

AnalyzerImpl::AnalyzerImpl(Disassembler *disassembler): m_document(disassembler->document()), m_disassembler(disassembler) { }

bool AnalyzerImpl::findNullSubs(const Symbol *symbol)
{
    const ListingItem* item = m_document->instructionItem(symbol->address);

    if(!item)
        return true; // Don't execute trampoline analysis

    InstructionPtr instruction = m_document->instruction(item->address());

    if(!instruction)
        return true; // Don't execute trampoline analysis

    if(!instruction->is(InstructionType::Stop))
        return false;

    m_document->lock(symbol->address, "nullsub_" + Utils::hex(symbol->address));
    return true;
}

void AnalyzerImpl::findTrampoline(const Symbol *symbol)
{
    ListingDocumentIterator it(m_document, symbol->address, ListingItemType::InstructionItem);

    if(!it.hasNext())
        return;

    const Assembler* assembler = m_disassembler->assembler();
    Symbol* symtrampoline = assembler->findTrampoline(&it);

    if(!symtrampoline)
        return;

    const Symbol* symentry = m_document->documentEntry();

    if(!symtrampoline->is(SymbolType::Import))
    {
        m_document->function(symtrampoline->address);

        if(!symbol->isLocked())
        {
            symtrampoline = m_document->symbol(symtrampoline->address); // Get updated symbol name from cache

            if(!symtrampoline)
                return;

            m_document->rename(symbol->address, Utils::trampoline(symtrampoline->name, "jmp_to"));
        }
        else if(symentry && (symbol->address == symentry->address))
        {
            m_document->lockFunction(symtrampoline->address, START_FUNCTION);
            m_document->setDocumentEntry(symtrampoline->address);
        }
        else
            return;
    }
    else if(symentry && (symbol->address != symentry->address))
        m_document->lock(symbol->address, Utils::trampoline(symtrampoline->name));
    else
        return;

    InstructionPtr instruction = m_document->instruction(symbol->address);

    if(!instruction)
        return;

    m_disassembler->pushReference(symtrampoline->address, instruction->address());
}

void AnalyzerImpl::checkFunctions()
{
    m_disassembler->document()->symbols()->iterate(SymbolType::FunctionMask, [this](const Symbol* symbol) -> bool {
        if(!this->findNullSubs(symbol))
            this->findTrampoline(symbol);

        return true;
    });
}

void AnalyzerImpl::loadSignatures()
{
    for(const std::string& signame : m_disassembler->loader()->signatures())
        m_disassembler->loadSignature(signame);
}

void AnalyzerImpl::analyzeFast()
{
    this->loadSignatures();
    this->checkFunctions();
}

} // namespace REDasm
