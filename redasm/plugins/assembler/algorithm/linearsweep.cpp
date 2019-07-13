#include "linearsweep.h"

namespace REDasm {

LinearSweepAlgorithm::LinearSweepAlgorithm(): Algorithm() { }

void LinearSweepAlgorithm::onDecoded(const CachedInstruction &instruction)
{
    Algorithm::onDecoded(instruction);
    this->enqueue(instruction->endAddress());
}

} // namespace REDasm
