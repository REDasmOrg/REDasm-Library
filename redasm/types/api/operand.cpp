#include "operand.h"
#include <cctype>

namespace REDasm {

bool Operand::isNumeric() const
{
    switch(this->type)
    {
        case OperandType::Constant:
        case OperandType::Immediate:
        case OperandType::Memory:
            return true;

        default: break;
    }

    return false;
}

bool Operand::isTarget() const { return REDasm::hasFlag(this, OperandFlags::Target); }
void Operand::asTarget() { this->flags |= OperandFlags::Target; }
bool Operand::isCharacter() const { return REDasm::typeIs(this, OperandType::Constant) && (this->u_value <= 0xFF) && ::isprint(static_cast<u8>(this->u_value)); }

bool Operand::checkCharacter()
{
    if(!REDasm::typeIs(this, OperandType::Immediate) || (this->u_value > 0xFF) || !::isprint(static_cast<u8>(this->u_value)))
        return false;

    this->type = OperandType::Constant;
    return true;
}

bool Operand::isIndexValid() const { return REDasm::typeIs(this, OperandType::Displacement) && REDasm::isValid(this->disp.indexstruct.r); }
bool Operand::isBaseValid() const { return REDasm::typeIs(this, OperandType::Displacement) && REDasm::isValid(this->disp.basestruct.r); }
bool Operand::displacementIsDynamic() const { return REDasm::typeIs(this, OperandType::Displacement) && (this->isBaseValid() || this->isIndexValid()); }
bool Operand::displacementCanBeAddress() const { return REDasm::typeIs(this, OperandType::Displacement) && (this->disp.displacement > 0); }

} // namespace REDasm
