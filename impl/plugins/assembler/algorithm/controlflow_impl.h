#pragma once

#include <redasm/plugins/assembler/algorithm/controlflow.h>
#include "algorithm_impl.h"

namespace REDasm {

class ControlFlowAlgorithmImpl: public AlgorithmImpl
{
    PIMPL_DECLARE_PUBLIC(ControlFlowAlgorithm)

    public:
        ControlFlowAlgorithmImpl(ControlFlowAlgorithm* algorithm);
        void enqueueTargets(const CachedInstruction &instruction);
};

} // namespace REDasm
