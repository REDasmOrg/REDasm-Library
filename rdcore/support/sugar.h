#pragma once

#include <rdapi/types.h>
#include <string>

class Sugar
{
    public:
        Sugar() = delete;
        static const RDOperand* target(const RDInstruction* instruction);
        static int branchDirection(const RDInstruction* instruction, address_t address);
        static address_t endAddress(const RDInstruction* instruction);
        static bool isBranch(const RDInstruction* instruction);
        static bool isUnconditionalJump(const RDInstruction* instruction);
        static bool isConditionalJump(const RDInstruction* instruction);
        static bool isNumeric(const RDOperand* operand);
        static bool isTarget(const RDOperand* operand);
        static bool isCharacter(const RDOperand* operand);
        static bool displacementIsDynamic(const RDOperand* operand);
        static bool displacementCanBeAddress(const RDOperand* operand);
        static bool isBaseValid(const RDOperand* operand);
        static bool isIndexValid(const RDOperand* operand);
};

