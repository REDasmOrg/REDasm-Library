#include "cachedinstruction_impl.h"
#include "backend/instructioncache.h"

namespace REDasm {

CachedInstructionImpl::CachedInstructionImpl(CachedInstruction *q): m_pimpl_q(q) { }
CachedInstructionImpl::CachedInstructionImpl(CachedInstruction *q, InstructionCache *cache): m_pimpl_q(q), m_cache(cache) { }
CachedInstructionImpl::~CachedInstructionImpl() { if(m_cache && m_instruction) m_cache->serialize(m_instruction); }

} // namespace REDasm
