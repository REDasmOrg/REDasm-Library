#pragma once

#include <memory>
#include <redasm/disassembler/listing/cachedinstruction.h>

namespace REDasm {

class InstructionCache;

class CachedInstructionImpl
{
    PIMPL_DECLARE_Q(CachedInstruction)
    PIMPL_DECLARE_PUBLIC(CachedInstruction)

    public:
        CachedInstructionImpl(CachedInstruction* q);
        CachedInstructionImpl(CachedInstruction* q, InstructionCache* cache);
        ~CachedInstructionImpl();

    private:
        std::shared_ptr<Instruction> m_instruction;
        InstructionCache* m_cache{nullptr};

    friend class InstructionCache;
};

} // namespace REDasm
