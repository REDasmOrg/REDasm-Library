#pragma once

#include "assembler.h"

namespace REDasm {

class CapstoneAssemblerImpl;

class LIBREDASM_API CapstoneAssembler : public Assembler
{
    PIMPL_DECLARE_PRIVATE(CapstoneAssembler)

    public:
        CapstoneAssembler(int arch, int mode);
        bool decodeInstruction(const BufferView& view, const InstructionPtr& instruction) override;
        size_t handle() const;

    protected:
        void onDecoded(const InstructionPtr &instruction) override;
};

} // namespace REDasm
