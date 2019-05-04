#pragma once

#include "../../emulator/emulator_type.h"

namespace REDasm {

class MetaARMEmulator: public EmulatorT<u32>
{
    public:
        MetaARMEmulator(DisassemblerAPI* disassembler);
        void emulate(const InstructionPtr& instruction) override;

    protected:
        bool setTarget(const InstructionPtr& instruction) override;

    private:
        void emulateMath(const InstructionPtr& instruction);
        void emulateMov(const InstructionPtr& instruction);
        void emulateLdr(const InstructionPtr& instruction);
        void emulateStr(const InstructionPtr& instruction);
};

} // namespace REDasm
