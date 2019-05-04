#pragma once

#include "../../plugins/plugins.h"

namespace REDasm {

class CILAssembler : public AssemblerPlugin
{
    PLUGIN_NAME("CIL/MSIL")

    public:
        CILAssembler() = default;

    protected:
        bool decodeInstruction(const BufferView &view, const InstructionPtr& instruction) override;
};

DECLARE_ASSEMBLER_PLUGIN(CILAssembler, cil, 32)

} // namespace REDasm
