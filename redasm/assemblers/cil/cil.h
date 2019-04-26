#pragma once

#include "../../plugins/plugins.h"

namespace REDasm {

class CILAssembler : public AssemblerPlugin
{
    PLUGIN_NAME("CIL/MSIL")

    public:
        CILAssembler() = default;

    protected:
        virtual bool decodeInstruction(const BufferView &view, const InstructionPtr& instruction);
};

DECLARE_ASSEMBLER_PLUGIN(CILAssembler, cil, 32)

} // namespace REDasm
