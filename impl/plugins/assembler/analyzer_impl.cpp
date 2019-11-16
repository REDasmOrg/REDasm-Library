#include "analyzer_impl.h"
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/plugins/loader/loader.h>
#include <redasm/disassembler/disassembler.h>
#include <redasm/support/utils.h>
#include <redasm/context.h>

namespace REDasm {

bool AnalyzerImpl::findNullSubs(address_t address)
{
    CachedInstruction instruction = r_doc->instruction(address);
    if(!instruction) return true; // Don't execute trampoline analysis
    if(!instruction->typeIs(InstructionType::Stop)) return false;

    r_doc->function(address, "nullsub_" + String::hex(address));
    return true;
}

void AnalyzerImpl::findTrampoline(address_t address)
{
    size_t index = r_doc->itemInstructionIndex(address);
    if(index == REDasm::npos) return;

    const Symbol* symtrampoline = r_asm->findTrampoline(index);
    if(!symtrampoline) return;

    const Symbol* symentry = r_doc->entry();
    if(symentry->address == address) return;

    if(symtrampoline->is(SymbolType::ImportNew))
        r_doc->rename(address, Utils::trampoline(symtrampoline->name));
    else
        r_doc->rename(address, Utils::trampoline(symtrampoline->name, "jmp_to"));

    r_disasm->pushReference(symtrampoline->address, address);
}

void AnalyzerImpl::checkFunctions()
{
    for(size_t i = 0; i < r_doc->functionsCount(); i++)
    {
        address_t address = r_doc->functionAt(i);
        if(this->findNullSubs(address)) continue;
        this->findTrampoline(address);
    }
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
