#pragma once

#include "algorithm.h"

class ControlFlowAlgorithm : public Algorithm
{
    public:
        ControlFlowAlgorithm();

    protected:
        void onDecoded(const RDInstruction* instruction) override;
        void enqueueTargets(const RDInstruction* instruction);
        void addressTableState(const RDState* state) override;
};
