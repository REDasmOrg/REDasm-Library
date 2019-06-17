#include "linearsweep.h"

namespace REDasm {

LinearSweepAlgorithm::LinearSweepAlgorithm(Disassembler *disassembler): Algorithm(disassembler) { }

void LinearSweepAlgorithm::onDecoded(Instruction *instruction)
{
    Algorithm::onDecoded(instruction);
    this->enqueue(instruction->endAddress());
}

} // namespace REDasm
