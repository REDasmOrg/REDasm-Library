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
        void onEmulatedOperand(const Operand *op, Instruction* instruction, u64 value) override;
        void onDecoded(Instruction* instruction) override;
        virtual void enqueueTarget(address_t target, Instruction* frominstruction);
};

} // namespace REDasm
