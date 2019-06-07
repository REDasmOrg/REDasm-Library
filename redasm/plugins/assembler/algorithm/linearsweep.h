#pragma once

#include "algorithm.h"

namespace REDasm {

class LinearSweepAlgorithm: public Algorithm
{
    public:
        LinearSweepAlgorithm(Disassembler* disassembler);

    protected:
        void onDecoded(const InstructionPtr& instruction) override;
};

} // namespace REDasm
