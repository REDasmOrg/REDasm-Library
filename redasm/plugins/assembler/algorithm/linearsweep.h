#pragma once

#include "algorithm.h"

namespace REDasm {

class LinearSweepAlgorithm: public AssemblerAlgorithm
{
    public:
        LinearSweepAlgorithm(DisassemblerAPI* disassembler, AssemblerPlugin* assemblerplugin);

    protected:
        virtual void onDecoded(const InstructionPtr& instruction);
};

} // namespace REDasm
