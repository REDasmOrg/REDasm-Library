#include "operand.h"
#include <cctype>

namespace REDasm {

bool Operand::isNumeric() const
{
    switch(this->type)
    {
        case Operand::T_Constant:
        case Operand::T_Immediate:
        case Operand::T_Memory:
            return true;

        default: break;
    }

    return false;
}

bool Operand::isTarget() const { return REDasm::hasFlag(this, Operand::F_Target); }
void Operand::asTarget() { this->flags |= Operand::F_Target; }
bool Operand::isConstant() const { return REDasm::typeIs(this, Operand::T_Constant); }
bool Operand::isRegister() const { return REDasm::typeIs(this, Operand::T_Register); }
bool Operand::isImmediate() const { return REDasm::typeIs(this, Operand::T_Immediate); }
bool Operand::isMemory() const { return REDasm::typeIs(this, Operand::T_Memory); }
bool Operand::isDisplacement() const { return REDasm::typeIs(this, Operand::T_Displacement); }
bool Operand::isCharacter() const { return REDasm::typeIs(this, Operand::T_Constant) && (this->u_value <= 0xFF) && ::isprint(static_cast<u8>(this->u_value)); }

bool Operand::checkCharacter()
{
    if(!REDasm::typeIs(this, Operand::T_Immediate) || (this->u_value > 0xFF) || !::isprint(static_cast<u8>(this->u_value)))
        return false;

    this->type = Operand::T_Constant;
    return true;
}

bool Operand::isIndexValid() const { return REDasm::typeIs(this, Operand::T_Displacement) && REDasm::isValid(this->disp.indexstruct.r); }
bool Operand::isBaseValid() const { return REDasm::typeIs(this, Operand::T_Displacement) && REDasm::isValid(this->disp.basestruct.r); }
bool Operand::displacementIsDynamic() const { return REDasm::typeIs(this, Operand::T_Displacement) && (this->isBaseValid() || this->isIndexValid()); }
bool Operand::displacementCanBeAddress() const { return REDasm::typeIs(this, Operand::T_Displacement) && (this->disp.displacement > 0); }

} // namespace REDasm
