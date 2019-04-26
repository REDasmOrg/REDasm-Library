#pragma once

#include "algorithm.h"

namespace REDasm {

class ControlFlowAlgorithm: public AssemblerAlgorithm
{
    public:
        ControlFlowAlgorithm(DisassemblerAPI* disassembler, AssemblerPlugin* assemblerplugin);

    protected:
        virtual void addressTableState(const State* state);
        virtual void enqueueTarget(address_t target, const InstructionPtr& frominstruction);
        virtual void onEmulatedOperand(const Operand *op, const InstructionPtr& instruction, u64 value);
        virtual void onDecoded(const InstructionPtr& instruction);

    private:
        void enqueueTargets(const InstructionPtr& instruction);
};

} // namespace REDasm
