#include "sugar.h"
#include <type_traits>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <cctype>

int Sugar::branchDirection(const RDInstruction* instruction, rd_address address)
{
    return static_cast<int>(static_cast<std::make_signed<decltype(address)>::type>(address) -
                            static_cast<std::make_signed<decltype(instruction->address)>::type>(instruction->address));
}

void Sugar::setMnemonic(RDInstruction* instruction, const char* mnemonic)
{
    if(!mnemonic) return;
    std::copy_n(mnemonic, std::min<size_t>(std::strlen(mnemonic) + 1, sizeof(instruction->mnemonic)), instruction->mnemonic);
}

rd_address Sugar::nextAddress(const RDInstruction* instruction) { return instruction->address + instruction->size; }
bool Sugar::isBranch(const RDInstruction* instruction) { return IS_TYPE(instruction, InstructionType_Call) || IS_TYPE(instruction, InstructionType_Jump); }

bool Sugar::isUnconditionalJump(const RDInstruction* instruction)
{
    if(!IS_TYPE(instruction, InstructionType_Jump)) return false;
    return !HAS_FLAG(instruction, InstructionFlags_Conditional);
}

bool Sugar::isConditionalJump(const RDInstruction* instruction)
{
    if(!IS_TYPE(instruction, InstructionType_Jump)) return false;
    return HAS_FLAG(instruction, InstructionFlags_Conditional);
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

bool Sugar::isCharacter(rd_address value) { return (value <= 0xFF) && ::isprint(static_cast<u8>(value)); }

bool Sugar::displacementCanBeAddress(const RDOperand* operand)
{
    return IS_TYPE(operand, OperandType_Displacement) && (operand->displacement > 0) && !Sugar::isBaseValid(operand) && !Sugar::isIndexValid(operand);
}

bool Sugar::isBaseValid(const RDOperand* operand)
{
    return IS_TYPE(operand, OperandType_Displacement) && (operand->base != RD_NREG);
}

bool Sugar::isIndexValid(const RDOperand* operand)
{
    return IS_TYPE(operand, OperandType_Displacement) && (operand->index != RD_NREG);
}
