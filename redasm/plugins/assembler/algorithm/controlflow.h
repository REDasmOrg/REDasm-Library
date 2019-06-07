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
        void onEmulatedOperand(const Operand *op, const InstructionPtr& instruction, u64 value) override;
        void onDecoded(const InstructionPtr& instruction) override;
        virtual void enqueueTarget(address_t target, const InstructionPtr& frominstruction);
};

} // namespace REDasm
