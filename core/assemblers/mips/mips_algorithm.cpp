#include "mips_algorithm.h"
#include <mips.h>

namespace REDasm {

MIPSAlgorithm::MIPSAlgorithm(DisassemblerAPI *disassembler, AssemblerPlugin *assemblerplugin): ControlFlowAlgorithm(disassembler, assemblerplugin)
{
    m_delayslotinstructions.insert(MIPS_INS_J);
    m_delayslotinstructions.insert(MIPS_INS_JAL);
    m_delayslotinstructions.insert(MIPS_INS_JR);
}

void MIPSAlgorithm::onDecoded(const InstructionPtr &instruction)
{
    if(m_pendingdelayslots.find(instruction->address) != m_pendingdelayslots.end())
    {
        AssemblerAlgorithm::onDecoded(instruction);
        m_pendingdelayslots.erase(instruction->address);

        if(instruction->is(InstructionType::Stop))
            return;
    }

    ControlFlowAlgorithm::onDecoded(instruction);

    if(m_delayslotinstructions.find(instruction->id) != m_delayslotinstructions.end())
    {
        m_pendingdelayslots.insert(instruction->endAddress());
        this->enqueue(instruction->endAddress());
    }
}

} // namespace REDasm
