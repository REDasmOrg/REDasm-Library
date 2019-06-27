#pragma once

#include "../../pimpl.h"
#include "../../types/api.h"

namespace REDasm {

class InstructionCache;
class CachedInstructionImpl;

class CachedInstruction
{
    PIMPL_DECLARE_P(CachedInstruction)
    PIMPL_DECLARE_PRIVATE(CachedInstruction)

    private:
        CachedInstruction(InstructionCache* cache, Instruction* instruction);

    public:
        CachedInstruction();
        Instruction* get() const;
        size_t referenceCount() const;
        Instruction* operator->() const;
        operator bool() const;

    friend class InstructionCache;
};

} // namespace REDasm
