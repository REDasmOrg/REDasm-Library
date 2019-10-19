#include "cachedinstruction_impl.h"
#include "backend/instructioncache.h"

namespace REDasm {

CachedInstructionImpl::CachedInstructionImpl(CachedInstruction *q): m_pimpl_q(q) { }
CachedInstructionImpl::CachedInstructionImpl(CachedInstruction *q, InstructionCache *cache, Instruction *instruction): m_pimpl_q(q), m_instruction(instruction), m_cache(cache) { }
CachedInstructionImpl::~CachedInstructionImpl() { PIMPL_Q(CachedInstruction); if(m_cache && m_instruction) m_cache->deallocate(*q); }
Instruction *CachedInstructionImpl::get() const { return m_instruction.get(); }
size_t CachedInstructionImpl::referenceCount() const { return m_instruction.use_count(); }

} // namespace REDasm
