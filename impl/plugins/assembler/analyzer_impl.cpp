#include "analyzer_impl.h"
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/plugins/loader/loader.h>
#include <redasm/disassembler/disassembler.h>
#include <redasm/support/utils.h>
#include <redasm/context.h>

namespace REDasm {

bool AnalyzerImpl::findNullSubs(address_t address)
{
    CachedInstruction instruction = r_docnew->instruction(address);
    if(!instruction) return true; // Don't execute trampoline analysis
    if(!instruction->is(InstructionType::Stop)) return false;

    r_docnew->function(address, "nullsub_" + String::hex(address));
    return true;
}

void AnalyzerImpl::findTrampoline(address_t address)
{
    size_t index = r_docnew->itemInstructionIndex(address);
    if(index == REDasm::npos) return;

    const Symbol* symtrampoline = r_asm->findTrampoline(index);
    if(!symtrampoline) return;

    const Symbol* symentry = r_docnew->entry();
    if(symentry->address == address) return;

    if(symtrampoline->is(SymbolType::ImportNew))
        r_docnew->rename(address, Utils::trampoline(symtrampoline->name));
    else
        r_docnew->rename(address, Utils::trampoline(symtrampoline->name, "jmp_to"));

    r_disasm->pushReference(symtrampoline->address, address);
}

void AnalyzerImpl::checkFunctions()
{
    const auto* functions = r_docnew->functions();

    for(size_t i = 0; i < functions->size(); i++)
    {
        if(this->findNullSubs(functions->at(i))) continue;
        this->findTrampoline(functions->at(i));
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
