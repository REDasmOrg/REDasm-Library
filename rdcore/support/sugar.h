#pragma once

#include <rdapi/types.h>
#include <string>

class Sugar
{
    public:
        Sugar() = delete;
        static rd_address nextAddress(const RDInstruction* instruction);
        static int branchDirection(const RDInstruction* instruction, rd_address address);
        static bool displacementCanBeAddress(const RDOperand* operand);
        static bool isBranch(const RDInstruction* instruction);
        static bool isUnconditionalJump(const RDInstruction* instruction);
        static bool isConditionalJump(const RDInstruction* instruction);
        static bool isNumeric(const RDOperand* operand);
        static bool isCharacter(rd_address value);
        static bool isBaseValid(const RDOperand* operand);
        static bool isIndexValid(const RDOperand* operand);
};

