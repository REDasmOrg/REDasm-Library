#include "api.h"
#include <impl/types/api_impl.h>
#include <impl/libs/cereal/cereal.hpp>
#include <impl/libs/cereal/archives/binary.hpp>
#include <cstring>

namespace REDasm {

REGISTER_FACTORY_OBJECT(Segment)
REGISTER_FACTORY_OBJECT(Operand)
REGISTER_FACTORY_OBJECT(Instruction)

Segment::Segment(): offset(0), address(0), endaddress(0), type(SegmentType::None) { }
Segment::Segment(const String &name, offset_t offset, address_t address, u64 psize, u64 vsize, SegmentType type): name(name), offset(offset), endoffset(offset + psize), address(address), endaddress(address + vsize), type(type) { }
u64 Segment::size() const { return (address > endaddress) ? 0 : (endaddress - address); }
u64 Segment::rawSize() const { return (offset > endoffset) ? 0 : (endoffset - offset); }
bool Segment::empty() const { return !this->size(); }
bool Segment::contains(address_t address) const { return (address >= this->address) && (address < endaddress); }
bool Segment::containsOffset(offset_t offset) const { return !this->is(SegmentType::Bss) && ((offset >= this->offset) && (offset < this->endoffset)); }
bool Segment::is(SegmentType t) const { return type & t; }
bool Segment::isPureCode() const { return type == SegmentType::Code; }

RegisterOperand::RegisterOperand(): r(REDasm::npos), tag(0) { }
RegisterOperand::RegisterOperand(register_id_t r): r(r), tag(0) { }
RegisterOperand::RegisterOperand(register_id_t r, tag_t tag): r(r), tag(tag) { }
bool RegisterOperand::isValid() const { return r != REDasm::npos; }
void RegisterOperand::save(cereal::BinaryOutputArchive &a) const { a(r, tag); }
void RegisterOperand::load(cereal::BinaryInputArchive &a) { a(r, tag); }

DisplacementOperand::DisplacementOperand(): scale(1), displacement(0) { }
DisplacementOperand::DisplacementOperand(const RegisterOperand &base, const RegisterOperand &index, s64 scale, s64 displacement): base(base), index(index), scale(scale), displacement(displacement) { }
void DisplacementOperand::save(cereal::BinaryOutputArchive &a) const { a(base, index, scale, displacement); }
void DisplacementOperand::load(cereal::BinaryInputArchive &a) { a(base, index, scale, displacement); }

Operand::Operand(): type(OperandType::None), tag(0), size(0), index(REDasm::npos), loc_index(REDasm::npos), u_value(0) { }
Operand::Operand(OperandType type, s64 value, size_t idx, tag_t tag): type(type), tag(tag), size(0), index(idx), loc_index(REDasm::npos), s_value(value) { }
Operand::Operand(OperandType type, u64 value, size_t idx, tag_t tag): type(type), tag(tag), size(0), index(idx), loc_index(REDasm::npos), u_value(value) { }
bool Operand::displacementIsDynamic() const { return is(OperandType::Displacement) && (disp.base.isValid() || disp.index.isValid()); }
bool Operand::displacementCanBeAddress() const { return is(OperandType::Displacement) && (disp.displacement > 0); }
bool Operand::isCharacter() const { return is(OperandType::Constant) && (u_value <= 0xFF) && ::isprint(static_cast<u8>(u_value)); }
bool Operand::isNumeric() const { return is(OperandType::Constant) || is(OperandType::Immediate) || is(OperandType::Memory); }
bool Operand::isTarget() const { return type & OperandType::Target; }
bool Operand::is(OperandType t) const { return type & t; }
void Operand::asTarget() { type |= OperandType::Target; }

bool Operand::checkCharacter()
{
    if(!is(OperandType::Immediate) || (u_value > 0xFF) || !::isprint(static_cast<u8>(u_value)))
        return false;

    type = OperandType::Constant;
    return true;
}

void Operand::save(cereal::BinaryOutputArchive &a) const { a(type, tag, size, index, loc_index, reg, disp, u_value); }
void Operand::load(cereal::BinaryInputArchive &a) { a(type, tag, size, index, loc_index, reg, disp, u_value); }

Instruction::Instruction(): m_pimpl_p(new InstructionImpl(this)), id(0), type(InstructionType::None), address(0), size(0) { }
address_t Instruction::endAddress() const { return address + size; }
bool Instruction::hasOperands() const { PIMPL_P(const Instruction); return p->hasOperands(); }
bool Instruction::contains(address_t address) const { return (address >= this->address) && (address < this->endAddress()); }
size_t Instruction::operandsCount() const { PIMPL_P(const Instruction); return p->operandsCount(); }
const Operand *Instruction::op(size_t idx) const { PIMPL_P(const Instruction); return p->op(idx); }
const Operand *Instruction::firstOperand() const { return this->hasOperands() ? this->op(0) : nullptr; }
const Operand *Instruction::lastOperand() const { return this->hasOperands() ? this->op(this->operandsCount() - 1) : nullptr; }
Operand *Instruction::op(size_t idx) { PIMPL_P(Instruction); return p->op(idx); }
Operand *Instruction::firstOperand() { return this->hasOperands() ? this->op(0) : nullptr; }
Operand *Instruction::lastOperand() { return this->hasOperands() ? this->op(this->operandsCount() - 1) : nullptr; }
const Operand *Instruction::target() const { PIMPL_P(const Instruction); return p->target(); }
Instruction *Instruction::mem(address_t v, tag_t tag) { PIMPL_P(Instruction); return p->mem(v, tag); }
Instruction *Instruction::cnst(u64 v, tag_t tag) { PIMPL_P(Instruction); return p->cnst(v, tag); }
Instruction *Instruction::imm(u64 v, tag_t tag) { PIMPL_P(Instruction); return p->imm(v, tag); }
Instruction *Instruction::disp(register_id_t base, s64 displacement) { PIMPL_P(Instruction); return p->disp(base, displacement); }
Instruction *Instruction::disp(register_id_t base, register_id_t index, s64 displacement) { PIMPL_P(Instruction); return p->disp(base, index, displacement); }
Instruction *Instruction::disp(register_id_t base, register_id_t index, s64 scale, s64 displacement) { PIMPL_P(Instruction); return p->disp(base, index, scale, displacement); }
Instruction *Instruction::arg(size_t locindex, register_id_t base, register_id_t index, s64 displacement) { PIMPL_P(Instruction); return p->arg(locindex, base, index, displacement); }
Instruction *Instruction::local(size_t locindex, register_id_t base, register_id_t index, s64 displacement, OperandType type) { PIMPL_P(Instruction); return p->local(locindex, base, index, displacement, type); }
Instruction *Instruction::reg(register_id_t r, tag_t tag) { PIMPL_P(Instruction); return p->reg(r, tag); }
Instruction *Instruction::tgt(address_t a) { PIMPL_P(Instruction); return p->tgt(a); }
const List &Instruction::targets() const { PIMPL_P(const Instruction); return p->targets(); }
void Instruction::target(address_t address) { PIMPL_P(Instruction); p->target(address); }
void Instruction::targetIdx(size_t idx) { PIMPL_P(Instruction); p->targetIdx(idx); }
bool Instruction::is(InstructionType t) const { return type & t; }
bool Instruction::is(const char *s) const { return mnemonic == s; }
bool Instruction::isInvalid() const { return type == InstructionType::Invalid; }
void *Instruction::userData() const { PIMPL_P(const Instruction); return p->userData(); }
void Instruction::reset() { PIMPL_P(Instruction); p->reset(); }
void Instruction::setFree(const Callback_VoidPointer &cb) { PIMPL_P(Instruction); p->setFree(cb); }
void Instruction::setUserData(void *userdata) { PIMPL_P(Instruction); p->setUserData(userdata); }

void Instruction::save(cereal::BinaryOutputArchive &a) const
{
    PIMPL_P(const Instruction);
    a(mnemonic, address, type, size, id, p->m_operands);
}

void Instruction::load(cereal::BinaryInputArchive &a)
{
    PIMPL_P(Instruction);
    a(mnemonic, address, type, size, id, p->m_operands);
}

} // namespace REDasm
