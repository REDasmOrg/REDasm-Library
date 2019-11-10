#include "cachedinstruction.h"
#include <impl/disassembler/listing/cachedinstruction_impl.h>

namespace REDasm {

CachedInstruction::CachedInstruction(): m_pimpl_p(new CachedInstructionImpl(this)) { }
CachedInstruction::CachedInstruction(InstructionCache *cache): m_pimpl_p(new CachedInstructionImpl(this, cache)) { }

void CachedInstruction::invalidate()
{
    PIMPL_P(CachedInstruction);
    p->m_cache = nullptr;
    p->m_instruction = nullptr;
}

Instruction* CachedInstruction::operator->() const { return this->get(); }
REDasm::CachedInstruction::operator bool() const { PIMPL_P(const CachedInstruction); return p && (p->m_instruction != nullptr); }
Instruction *CachedInstruction::get() const { PIMPL_P(const CachedInstruction); return p->m_instruction.get(); }

} // namespace REDasm
