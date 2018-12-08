#ifndef MIPS_ALGORITHM_H
#define MIPS_ALGORITHM_H

#include "../../plugins/assembler/algorithm/controlflow.h"

namespace REDasm {

class MIPSAlgorithm : public ControlFlowAlgorithm
{
    public:
        MIPSAlgorithm(DisassemblerAPI* disassembler, AssemblerPlugin* assemblerplugin);

    protected:
        virtual void onDecoded(const InstructionPtr& instruction);

    private:
        std::set<address_t> m_pendingdelayslots;
        std::set<instruction_id_t> m_delayslotinstructions;
};

} // namespace REDasm

#endif // MIPS_ALGORITHM_H
