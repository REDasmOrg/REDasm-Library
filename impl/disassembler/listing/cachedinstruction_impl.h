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
        CachedInstructionImpl(CachedInstruction* q, InstructionCache* cache, Instruction* instruction);
        ~CachedInstructionImpl();
        Instruction* get() const;
        size_t referenceCount() const;

    private:
        std::shared_ptr<Instruction> m_instruction;
        InstructionCache* m_cache{nullptr};
};

} // namespace REDasm
