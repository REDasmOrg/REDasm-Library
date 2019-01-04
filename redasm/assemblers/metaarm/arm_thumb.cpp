#include "arm_thumb.h"
#include "arm_common.h"

namespace REDasm {

ARMThumbAssembler::ARMThumbAssembler(): ARMCommonAssembler<CS_ARCH_ARM, CS_MODE_THUMB>() { }
const char *ARMThumbAssembler::name() const { return "ARM Thumb mode"; }

u64 ARMThumbAssembler::pc(const InstructionPtr &instruction) const
{
    /*
     * https://stackoverflow.com/questions/24091566/why-does-the-arm-pc-register-point-to-the-instruction-after-the-next-one-to-be-e
     *
     * In Thumb state:
     *  - For B, BL, CBNZ, and CBZ instructions, the value of the PC is the address
     *    of the current instruction plus 4 bytes.
     *
     *  - For all other instructions that use labels, the value of the PC is the address
     *    of the current instruction plus 4 bytes, with bit[1] of the result cleared
     *    to 0 to make it word-aligned.
     */

    switch(instruction->id)
    {
        case ARM_INS_B:
        case ARM_INS_BL:
        case ARM_INS_CBNZ:
        case ARM_INS_CBZ:
            return instruction->address + 4;

        default:
            break;
    }

    return (instruction->address + 4) & 0xFFFFFFFE;
}

} // namespace REDasm
