#include "operand.h"
#include <cctype>

namespace REDasm {

bool Operand::isNumeric(const Operand* op)
{
    switch(op->type)
    {
        case OperandType::Constant:
        case OperandType::Immediate:
        case OperandType::Memory:
            return true;

        default: break;
    }

    return false;
}

bool Operand::isTarget(const Operand* op) { return REDasm::hasFlag(op, OperandFlags::Target); }
void Operand::asTarget(Operand* op) { op->flags |= OperandFlags::Target; }
bool Operand::isCharacter(const Operand* op) { return REDasm::typeIs(op, OperandType::Constant) && (op->u_value <= 0xFF) && ::isprint(static_cast<u8>(op->u_value)); }

bool Operand::checkCharacter(Operand* op)
{
    if(!REDasm::typeIs(op, OperandType::Immediate) || (op->u_value > 0xFF) || !::isprint(static_cast<u8>(op->u_value)))
        return false;

    op->type = OperandType::Constant;
    return true;
}

bool Operand::isIndexValid(const REDasm::Operand* op) { return REDasm::typeIs(op, OperandType::Displacement) && REDasm::isValid(op->disp.indexstruct.r); }
bool Operand::isBaseValid(const REDasm::Operand* op) { return REDasm::typeIs(op, OperandType::Displacement) && REDasm::isValid(op->disp.basestruct.r); }
bool Operand::displacementIsDynamic(const Operand* op) { return REDasm::typeIs(op, OperandType::Displacement) && (Operand::isBaseValid(op) || Operand::isIndexValid(op)); }
bool Operand::displacementCanBeAddress(const Operand* op) { return REDasm::typeIs(op, OperandType::Displacement) && (op->disp.displacement > 0); }

} // namespace REDasm
