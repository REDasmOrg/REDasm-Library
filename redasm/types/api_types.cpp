#include "api_types.h"
#include <impl/types/api_types_impl.h>
#include <cstring>

namespace REDasm {

Instruction::Instruction(): m_pimpl_p(new InstructionImpl(this)) { }
const char *Instruction::mnemonic() const { PIMPL_P(const Instruction); return p->mnemonic(); }
address_t Instruction::address() const { PIMPL_P(const Instruction); return p->address(); }
address_t Instruction::endAddress() const { PIMPL_P(const Instruction); return p->endAddress(); }
u32 Instruction::size() const { PIMPL_P(const Instruction); return p->size(); }
 u32 &Instruction::size() { PIMPL_P(Instruction); return p->size(); }
instruction_id_t Instruction::id() const { PIMPL_P(const Instruction); return p->id(); }
const InstructionType& Instruction::type() const { PIMPL_P(const Instruction); return p->type();  }
InstructionType &Instruction::type() { PIMPL_P(Instruction); return p->type(); }
bool Instruction::hasOperands() const { return this->operandsCount(); }
size_t Instruction::operandsCount() const { PIMPL_P(const Instruction); return p->operandsCount(); }
size_t Instruction::opSize(size_t idx) const { PIMPL_P(const Instruction); return p->opSize(idx); }
const Operand *Instruction::op(size_t idx) const { PIMPL_P(const Instruction); return p->op(idx); }
Operand *Instruction::op(size_t idx) { PIMPL_P(Instruction); return p->op(idx); }
const Operand *Instruction::target() const { PIMPL_P(const Instruction); return p->target(); }
void Instruction::opSize(size_t idx, size_t size) { PIMPL_P(Instruction); p->opSize(idx, size); }
Instruction *Instruction::mem(address_t v, tag_t tag) { PIMPL_P(Instruction); return p->mem(v, tag); }
Instruction *Instruction::cnst(u64 v, tag_t tag) { PIMPL_P(Instruction); return p->cnst(v, tag); }
Instruction *Instruction::imm(u64 v, tag_t tag) { PIMPL_P(Instruction); return p->imm(v, tag); }
Instruction *Instruction::disp(register_id_t base, s64 displacement) { PIMPL_P(Instruction); return p->disp(base, displacement); }
Instruction *Instruction::disp(register_id_t base, register_id_t index, s64 displacement) { PIMPL_P(Instruction); return p->disp(base, index, displacement); }
Instruction *Instruction::disp(register_id_t base, register_id_t index, s64 scale, s64 displacement) { PIMPL_P(Instruction); return p->disp(base, index, scale, displacement); }
Instruction *Instruction::arg(size_t locindex, register_id_t base, register_id_t index, s64 displacement) { PIMPL_P(Instruction); return p->arg(locindex, base, index, displacement); }
Instruction *Instruction::local(size_t locindex, register_id_t base, register_id_t index, s64 displacement, OperandType type) { PIMPL_P(Instruction); return p->local(locindex, base, index, displacement, type); }
Instruction *Instruction::reg(register_id_t r, tag_t tag) { PIMPL_P(Instruction); return p->reg(r, tag); }
size_t Instruction::targetsCount() const { PIMPL_P(const Instruction); return p->targetsCount(); }
address_t Instruction::targetAt(size_t idx) const { PIMPL_P(const Instruction); return p->targetAt(idx); }
void Instruction::target(address_t address) { PIMPL_P(Instruction); p->target(address); }
void Instruction::targetIdx(size_t idx) { PIMPL_P(Instruction); p->targetIdx(idx); }
void Instruction::clearTargets() { PIMPL_P(Instruction); p->clearTargets(); }
bool Instruction::is(InstructionType t) const { PIMPL_P(const Instruction); return p->is(t); }
bool Instruction::is(const char *s) const { PIMPL_P(const Instruction); return p->m_mnemonic == s; }
bool Instruction::isInvalid() const { PIMPL_P(const Instruction); return p->isInvalid(); }
void *Instruction::userData() const { PIMPL_P(const Instruction); return p->userData(); }
void Instruction::reset() { PIMPL_P(Instruction); p->reset(); }
void Instruction::setMnemonic(const char *s) { PIMPL_P(Instruction); p->setMnemonic(s); }
void Instruction::setAddress(address_t address) { PIMPL_P(Instruction); p->setAddress(address); }
void Instruction::setSize(u32 size) { PIMPL_P(Instruction); p->setSize(size); }
void Instruction::setId(instruction_id_t id) { PIMPL_P(Instruction); p->setId(id); }
void Instruction::setType(InstructionType type) { PIMPL_P(Instruction); p->setType(type); }
void Instruction::setFree(const Callback_VoidPointer &cb) { PIMPL_P(Instruction); p->setFree(cb); }
void Instruction::setUserData(void *userdata) { PIMPL_P(Instruction); p->setUserData(userdata); }

void REDasm::Serializer<Instruction>::write(std::fstream &fs, const Instruction &st)
{
    Serializer<std::string>::write(fs, st.pimpl_p()->m_mnemonic);
    Serializer<decltype(st.pimpl_p()->m_operands)>::write(fs, st.pimpl_p()->m_operands);
    Serializer<address_t>::write(fs, st.pimpl_p()->m_address);
    Serializer<InstructionType>::write(fs, st.pimpl_p()->m_type);
    Serializer<u32>::write(fs, st.pimpl_p()->m_size);
    Serializer<instruction_id_t>::write(fs, st.pimpl_p()->m_id);
}

void REDasm::Serializer<Instruction>::read(std::fstream &fs, Instruction &st)
{
    Serializer<std::string>::read(fs, st.pimpl_p()->m_mnemonic);
    Serializer<decltype(st.pimpl_p()->m_operands)>::read(fs, st.pimpl_p()->m_operands);
    Serializer<address_t>::read(fs, st.pimpl_p()->m_address);
    Serializer<InstructionType>::read(fs, st.pimpl_p()->m_type);
    Serializer<u32>::read(fs, st.pimpl_p()->m_size);
    Serializer<instruction_id_t>::read(fs, st.pimpl_p()->m_id);
}

} // namespace REDasm
