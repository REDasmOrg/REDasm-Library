#include "linearsweep.h"

namespace REDasm {

LinearSweepAlgorithm::LinearSweepAlgorithm(DisassemblerAPI *disassembler, AssemblerPlugin *assemblerplugin): AssemblerAlgorithm(disassembler, assemblerplugin) { }

void LinearSweepAlgorithm::onDecoded(const InstructionPtr &instruction)
{
    AssemblerAlgorithm::onDecoded(instruction);
    this->enqueue(instruction->endAddress());
}

} // namespace REDasm
