#ifndef CIL_H
#define CIL_H

#include "../../plugins/plugins.h"

namespace REDasm {

class CILAssembler : public AssemblerPlugin
{
    PLUGIN_NAME("CIL/MSIL")

    public:
        CILAssembler() = default;
        virtual u32 bits() const;

    protected:
        virtual bool decodeInstruction(const BufferView &view, const InstructionPtr& instruction);
};

DECLARE_ASSEMBLER_PLUGIN(CILAssembler, cil)

} // namespace REDasm

#endif // CIL_H
