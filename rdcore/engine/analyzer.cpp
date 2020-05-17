#include "analyzer.h"
#include "../support/utils.h"
#include "../support/sugar.h"

Analyzer::Analyzer(Disassembler* disassembler): m_disassembler(disassembler) { }

void Analyzer::analyze()
{
    this->analyzeDefault(); // Integrated analysis

    if(m_loaderdone) return;
    m_loaderdone = true;

    if(m_disassembler->loader()->analyze(m_disassembler)) // Try with custom analysis
        this->analyzeDefault();
}

void Analyzer::analyzeDefault() { this->checkFunctions(); }

void Analyzer::checkFunctions()
{
    auto& document = m_disassembler->document();
    auto lock = x_lock_safe_ptr(document);

    for(size_t i = 0; i < lock->functionsCount(); i++)
    {
        RDLocation loc = lock->functionAt(i);
        if(this->findNullSubs(lock, loc.address)) continue;
        this->findTrampoline(lock, loc.address);
    }
}

bool Analyzer::findNullSubs(x_lock_document& lock, address_t address)
{
    RDInstruction* instruction = nullptr;
    if(!lock->lockInstruction(address, &instruction)) return true; // Don't execute trampoline analysis

    bool isnullsub = HAS_FLAG(instruction, InstructionFlags_Stop);
    if(isnullsub) lock->rename(address, "nullsub_" + Utils::hex(address));
    lock->unlockInstruction(instruction);
    return isnullsub;
}

void Analyzer::findTrampoline(x_lock_document& lock, address_t address)
{
    if(lock->entry()->address == address) return; // Don't rename EP, if any

    RDLocation loc{ };
    RDSymbol symbol{ };
    RDInstruction* instruction = nullptr;
    const char* name = nullptr;

    if(!lock->lockInstruction(address, &instruction)) return;
    if(!Sugar::isUnconditionalJump(instruction)) goto cleanup;

    loc = m_disassembler->getTarget(address);
    if(!loc.valid || !lock->symbol(loc.value, &symbol)) goto cleanup;
    if(!(name = lock->name(symbol.address))) goto cleanup;

    if(IS_TYPE(&symbol, SymbolType_Import)) lock->rename(address, Utils::trampoline(name));
    else lock->rename(address, "jmp_to_" + std::string(name));

cleanup:
    lock->unlockInstruction(instruction);
}
