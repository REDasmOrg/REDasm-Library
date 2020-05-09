#pragma once

#include <rdapi/types.h>
#include <string>

class Sugar
{
    public:
        Sugar() = delete;
        static address_t nextAddress(const RDInstruction* instruction);
        static int branchDirection(const RDInstruction* instruction, address_t address);
        static bool displacementCanBeAddress(const RDOperand* operand);
        static bool isBranch(const RDInstruction* instruction);
        static bool isUnconditionalJump(const RDInstruction* instruction);
        static bool isConditionalJump(const RDInstruction* instruction);
        static bool isNumeric(const RDOperand* operand);
        static bool isCharacter(address_t value);
        static bool isBaseValid(const RDOperand* operand);
        static bool isIndexValid(const RDOperand* operand);
};

