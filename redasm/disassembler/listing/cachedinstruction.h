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
        CachedInstruction(InstructionCache* cache);

    public:
        CachedInstruction();
        void invalidate();
        Instruction* get() const;
        Instruction* operator->() const;
        operator bool() const;

    friend class InstructionCache;
};

} // namespace REDasm
