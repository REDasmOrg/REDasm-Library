#include "sugar.h"
#include <type_traits>
#include <algorithm>
#include <iterator>
#include <cctype>

const RDOperand* Sugar::target(const RDInstruction* instruction)
{
    auto it = std::find_if(std::begin(instruction->operands), std::end(instruction->operands), [](const RDOperand& op) -> bool {
        return op.flags & OperandFlags_Target;
    });

    return (it != std::end(instruction->operands)) ? std::addressof(*it) : nullptr;
}

int Sugar::branchDirection(const RDInstruction* instruction, address_t address)
{
    return static_cast<int>(static_cast<std::make_signed<decltype(address)>::type>(address) -
                            static_cast<std::make_signed<decltype(instruction->address)>::type>(instruction->address));
}

address_t Sugar::endAddress(const RDInstruction* instruction) { return instruction->address + instruction->size; }
bool Sugar::isBranch(const RDInstruction* instruction) { return (instruction->type == InstructionType_Call) || (instruction->type == InstructionType_Jump); }

bool Sugar::isUnconditionalJump(const RDInstruction* instruction)
{
    if(instruction->type != InstructionType_Jump) return false;
    return !(instruction->flags & InstructionFlags_Conditional);
}

bool Sugar::isConditionalJump(const RDInstruction* instruction)
{
    if(instruction->type != InstructionType_Jump) return false;
    return instruction->flags & InstructionFlags_Conditional;
}

bool Sugar::isNumeric(const RDOperand* operand)
{
    switch(operand->type)
     {
        case OperandType_Constant:
        case OperandType_Immediate:
        case OperandType_Memory: return true;
        default: break;
    }

    return false;
}

bool Sugar::isTarget(const RDOperand* operand) { return operand->flags & OperandFlags_Target; }

bool Sugar::isCharacter(const RDOperand* operand)
{
    if(operand->type != OperandType_Constant) return false;
    return (operand->u_value <= 0xFF) && ::isprint(static_cast<u8>(operand->u_value));
}

bool Sugar::displacementIsDynamic(const RDOperand* operand)
{
    return (operand->type == OperandType_Displacement) && (Sugar::isBaseValid(operand) || Sugar::isIndexValid(operand));
}

bool Sugar::displacementCanBeAddress(const RDOperand* operand)
{
    return (operand->type == OperandType_Displacement) && (operand->displacement > 0);
}

bool Sugar::isBaseValid(const RDOperand* operand)
{
    return (operand->type == OperandType_Displacement) && (operand->base != RD_NREG);
}

bool Sugar::isIndexValid(const RDOperand* operand)
{
    return (operand->type == OperandType_Displacement) && (operand->index != RD_NREG);
}
