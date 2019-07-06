#pragma once

#include "algorithm_impl.h"
#include <redasm/plugins/assembler/algorithm/controlflow.h>

namespace REDasm {

class ControlFlowAlgorithmImpl: public AlgorithmImpl
{
    PIMPL_DECLARE_PUBLIC(ControlFlowAlgorithm)

    public:
        ControlFlowAlgorithmImpl(ControlFlowAlgorithm* algorithm, Disassembler* disassembler);
        void enqueueTargets(const CachedInstruction &instruction);
};

} // namespace REDasm
