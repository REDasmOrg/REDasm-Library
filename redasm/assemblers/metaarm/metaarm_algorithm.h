#pragma once

#include "../../plugins/assembler/algorithm/controlflow.h"

namespace REDasm {

class MetaARMAlgorithm : public ControlFlowAlgorithm
{
    public:
        MetaARMAlgorithm(DisassemblerAPI* disassembler, AssemblerPlugin* assemblerplugin);

    protected:
        virtual void onEmulatedOperand(const Operand* op, const InstructionPtr& instruction, u64 value);
        virtual void enqueueTarget(address_t target, const InstructionPtr& instruction);
        virtual void decodeState(const State *state);
        virtual void memoryState(const State* state);
        virtual void pointerState(const State* state);
        virtual void immediateState(const State *state);

    private:
        std::unordered_map<address_t, bool> m_armstate;

};

} // namespace REDasm
