#pragma once

#include "algorithm.h"

namespace REDasm {

class ControlFlowAlgorithm: public AssemblerAlgorithm
{
    public:
        ControlFlowAlgorithm(DisassemblerAPI* disassembler, AssemblerPlugin* assemblerplugin);

    protected:
        void addressTableState(const State* state) override;
        void onEmulatedOperand(const Operand *op, const InstructionPtr& instruction, u64 value) override;
        void onDecoded(const InstructionPtr& instruction) override;
        virtual void enqueueTarget(address_t target, const InstructionPtr& frominstruction);

    private:
        void enqueueTargets(const InstructionPtr& instruction);
};

} // namespace REDasm
