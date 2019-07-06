#pragma once

#include "algorithm.h"

namespace REDasm {

class ControlFlowAlgorithmImpl;

class ControlFlowAlgorithm: public Algorithm
{
    PIMPL_DECLARE_PRIVATE(ControlFlowAlgorithm)

    public:
        ControlFlowAlgorithm(Disassembler* disassembler);

    protected:
        void addressTableState(const State* state) override;
        void onEmulatedOperand(const Operand *op, const CachedInstruction& instruction, u64 value) override;
        void onDecoded(const CachedInstruction& instruction) override;
        virtual void enqueueTarget(address_t target, const CachedInstruction& frominstruction);
};

} // namespace REDasm
