#pragma once

#include <list>
#include "../../types/buffer/bufferview.h"
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
        static int classify(address_t address, const BufferView& view, DisassemblerAPI* disassembler, AssemblerPlugin* armassembler);

    private:
        static bool validateBranch(const InstructionPtr& instruction, DisassemblerAPI *disassembler);
};

} // namespace REDasm
