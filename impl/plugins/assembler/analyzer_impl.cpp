#include "analyzer_impl.h"
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/plugins/loader/loader.h>
#include <redasm/disassembler/disassembler.h>
#include <redasm/support/utils.h>
#include <redasm/context.h>

namespace REDasm {

bool AnalyzerImpl::findNullSubs(const Symbol *symbol)
{
    ListingItem item = r_docnew->itemInstruction(symbol->address);
    if(!item.isValid()) return true; // Don't execute trampoline analysis

    CachedInstruction instruction = r_doc->instruction(item.address_new);
    if(!instruction) return true; // Don't execute trampoline analysis
    if(!instruction->is(InstructionType::Stop)) return false;

    r_doc->lock(symbol->address, "nullsub_" + String::hex(symbol->address));
    return true;
}

void AnalyzerImpl::findTrampoline(const Symbol *symbol)
{
    size_t index = r_docnew->itemInstructionIndex(symbol->address);
    if(index == REDasm::npos) return;

    const Symbol* symtrampoline = r_asm->findTrampoline(index);
    if(!symtrampoline) return;

    const Symbol* symentry = r_docnew->entry();

    if(!symtrampoline->is(SymbolType::Import))
    {
        r_docnew->function(symtrampoline->address);

        if(!symbol->isLocked())
        {
            symtrampoline = r_docnew->symbol(symtrampoline->address); // Get updated symbol name from cache

            if(!symtrampoline)
                return;

            r_doc->rename(symbol->address, Utils::trampoline(symtrampoline->name, "jmp_to"));
        }
        else if(symentry && (symbol->address == symentry->address))
        {
            r_doc->lockFunction(symtrampoline->address, START_FUNCTION);
            r_docnew->entry(symtrampoline->address);
        }
        else
            return;
    }
    else if(symentry && (symbol->address != symentry->address))
        r_doc->lock(symbol->address, Utils::trampoline(symtrampoline->name));
    else
        return;

    CachedInstruction instruction = r_docnew->instruction(symbol->address);
    if(!instruction) return;

    r_disasm->pushReference(symtrampoline->address, instruction->address);
}

void AnalyzerImpl::checkFunctions()
{
    r_disasm->document()->symbols()->iterate(SymbolType::FunctionMask, [this](const Symbol* symbol) -> bool {
        if(!this->findNullSubs(symbol))
            this->findTrampoline(symbol);

        return true;
    });
}

void AnalyzerImpl::loadSignatures()
{
    for(const String& signame : r_disasm->loader()->signatures())
        r_disasm->loadSignature(signame);
}

void AnalyzerImpl::analyzeFast()
{
    this->loadSignatures();
    this->checkFunctions();
}

} // namespace REDasm
