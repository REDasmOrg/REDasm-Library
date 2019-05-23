#pragma once

#include "../../plugins/assembler/algorithm/controlflow.h"

namespace REDasm {

class MetaARMAlgorithm : public ControlFlowAlgorithm
{
    public:
        MetaARMAlgorithm(DisassemblerAPI* disassembler, AssemblerPlugin* assemblerplugin);

    protected:
        void onEmulatedOperand(const Operand* op, const InstructionPtr& instruction, u64 value) override;
        void enqueueTarget(address_t target, const InstructionPtr& instruction) override;
        void decodeState(const State *state) override;
        void memoryState(const State* state) override;
        void pointerState(const State* state) override;
        void immediateState(const State *state) override;

    private:
        std::unordered_map<address_t, bool> m_armstate;

};

} // namespace REDasm
