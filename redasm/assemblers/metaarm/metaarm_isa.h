#ifndef METAARM_ISA_H
#define METAARM_ISA_H

#include <list>
#include "../../redasm_buffer.h"
#include "../../plugins/assembler/assembler.h"

namespace REDasm {

class DisassemblerAPI;

class MetaARMAssemblerISA
{
    public:
        enum { ARM, Thumb };

    public:
        MetaARMAssemblerISA() = delete;
        MetaARMAssemblerISA(const MetaARMAssemblerISA&) = delete;
        static int classify(address_t address, const BufferRef& buffer, DisassemblerAPI* disassembler, AssemblerPlugin* armassembler);

    private:
        static bool validateBranch(const InstructionPtr& instruction, DisassemblerAPI *disassembler);
};

} // namespace REDasm

#endif // METAARM_ISA_H
