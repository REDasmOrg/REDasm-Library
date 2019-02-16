#include "arm.h"
#include "arm_common.h"
#include "metaarm_emulator.h"

namespace REDasm {

ARMAssembler::ARMAssembler(): ARMCommonAssembler<CS_ARCH_ARM, CS_MODE_ARM>() { }
std::string ARMAssembler::name() const { return "ARM"; }
u32 ARMAssembler::flags() const { return AssemblerFlags::HasEmulator; }
Emulator *ARMAssembler::createEmulator(DisassemblerAPI *disassembler) const { return new MetaARMEmulator(disassembler); }
Printer *ARMAssembler::createPrinter(DisassemblerAPI *disassembler) const { return new MetaARMPrinter(m_cshandle, disassembler); }

u64 ARMAssembler::pc(const InstructionPtr &instruction) const
{
    /*
     * https://stackoverflow.com/questions/24091566/why-does-the-arm-pc-register-point-to-the-instruction-after-the-next-one-to-be-e
     *
     * In ARM state:
     *  - The value of the PC is the address of the current instruction plus 8 bytes.
     */

    return instruction->address + 8;
}

} // namespace REDasm
