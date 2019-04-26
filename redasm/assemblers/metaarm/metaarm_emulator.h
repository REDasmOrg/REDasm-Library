#pragma once

#include "../../emulator/emulator_type.h"

namespace REDasm {

class MetaARMEmulator: public EmulatorT<u32>
{
    public:
        MetaARMEmulator(DisassemblerAPI* disassembler);
        virtual void emulate(const InstructionPtr& instruction);

    protected:
        virtual bool setTarget(const InstructionPtr& instruction);

    private:
        void emulateMath(const InstructionPtr& instruction);
        void emulateMov(const InstructionPtr& instruction);
        void emulateLdr(const InstructionPtr& instruction);
        void emulateStr(const InstructionPtr& instruction);
};

} // namespace REDasm
