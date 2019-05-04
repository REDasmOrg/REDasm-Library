#pragma once

#include "emulator_alu.h"
#include "../redasm.h"

namespace REDasm {

template<typename T> class EmulatorT: public EmulatorALU<T>
{
    public:
        EmulatorT(DisassemblerAPI* disassembler);
        bool read(const Operand* op, u64* value) override;

    protected:
        void moveOp(const InstructionPtr& instruction, int opdest, int opsrc);
};

} // namespace REDasm

#include "emulator_type.cpp"
