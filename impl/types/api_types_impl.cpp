#include "api_types_impl.h"

namespace REDasm {

InstructionImpl::InstructionImpl(Instruction *q): m_pimpl_q(q), m_free(nullptr), m_address(0), m_type(InstructionType::None), m_size(0), m_id(0) { m_userdata = nullptr; }
InstructionImpl::~InstructionImpl() { this->reset(); }
const char *InstructionImpl::mnemonic() const { return m_mnemonic.c_str(); }
address_t InstructionImpl::address() const { return m_address; }
address_t InstructionImpl::endAddress() const { return m_address + m_size; }
u32 InstructionImpl::size() const { return m_size; }
u32 &InstructionImpl::size() { return m_size; }
instruction_id_t InstructionImpl::id() const { return m_id; }
const InstructionType &InstructionImpl::type() const { return m_type; }
InstructionType &InstructionImpl::type() { return m_type; }
const Operand *InstructionImpl::op(size_t idx) const { return &m_operands.at(idx); }
Operand *InstructionImpl::op(size_t idx) { return &m_operands.at(idx); }

const Operand *InstructionImpl::target() const
{
    for(const Operand& op : m_operands)
    {
        if(op.isTarget())
            return &op;
    }

    return nullptr;
}

void InstructionImpl::opSize(size_t idx, size_t size) { m_operands[idx].size = size; }

Instruction *InstructionImpl::mem(address_t v, tag_t tag)
{
    PIMPL_Q(Instruction);
    m_operands.emplace_back(OperandType::Memory, v, m_operands.size(), tag);
    return q;
}

Instruction *InstructionImpl::cnst(u64 v, tag_t tag)
{
    PIMPL_Q(Instruction);
    m_operands.emplace_back(OperandType::Constant, v, m_operands.size(), tag);
    return q;
}

Instruction *InstructionImpl::imm(u64 v, tag_t tag)
{
    PIMPL_Q(Instruction);
    m_operands.emplace_back(OperandType::Immediate, v, m_operands.size(), tag);
    return q;
}

Instruction *InstructionImpl::disp(register_id_t base, s64 displacement) { return this->disp(base, REDasm::npos, displacement); }
Instruction *InstructionImpl::disp(register_id_t base, register_id_t index, s64 displacement) { return this->disp(base, index, 1, displacement);  }

Instruction *InstructionImpl::disp(register_id_t base, register_id_t index, s64 scale, s64 displacement)
{
    PIMPL_Q(Instruction);

    Operand op;
    op.index = m_operands.size();

    if((base == REDasm::npos) && (index == REDasm::npos))
    {
        op.type = OperandType::Memory;
        op.u_value = scale * displacement;
    }
    else
    {
        op.type = OperandType::Displacement;
        op.disp = DisplacementOperand(RegisterOperand(base), RegisterOperand(index), scale, displacement);
    }

    m_operands.emplace_back(op);
    return q;
}

Instruction *InstructionImpl::arg(size_t locindex, register_id_t base, register_id_t index, s64 displacement) { return this->local(locindex, base, index, displacement, OperandType::Argument); }

Instruction *InstructionImpl::local(size_t locindex, register_id_t base, register_id_t index, s64 displacement, OperandType type)
{
    PIMPL_Q(Instruction);

    Operand op;
    op.index = m_operands.size();
    op.loc_index = locindex;
    op.type = OperandType::Displacement | type;
    op.disp = DisplacementOperand(RegisterOperand(base), RegisterOperand(index), 1, displacement);

    m_operands.emplace_back(op);
    return q;
}

Instruction *InstructionImpl::reg(register_id_t r, tag_t tag)
{
    PIMPL_Q(Instruction);

    Operand op;
    op.index = m_operands.size();
    op.type = OperandType::Register;
    op.reg = RegisterOperand(r, tag);

    m_operands.emplace_back(op);
    return q;
}

size_t InstructionImpl::opSize(size_t idx) const { return m_operands[idx].size; }
size_t InstructionImpl::operandsCount() const { return m_operands.size(); }
size_t InstructionImpl::targetsCount() const { return m_targets.size(); }
address_t InstructionImpl::targetAt(size_t idx) const { return m_targets[idx]; }

void InstructionImpl::target(address_t address)
{
    auto it = m_utargets.find(address);

    if(it != m_utargets.end())
        return;

    m_targets.push_back(address);
}

void InstructionImpl::targetIdx(size_t idx)
{
    if(idx >= m_operands.size())
        return;

    m_operands[idx].asTarget();

    if(m_operands[idx].isNumeric())
        this->target(m_operands[idx].u_value);
}

void InstructionImpl::clearTargets() { m_targets.clear(); }
bool InstructionImpl::is(InstructionType t) const { return m_type & t; }
bool InstructionImpl::isInvalid() const { return m_type == InstructionType::Invalid; }
void *InstructionImpl::userData() const { return m_userdata; }

void InstructionImpl::reset()
{
    m_type = InstructionType::None;
    m_size = 0;
    m_operands.clear();

    if(m_free && m_userdata)
    {
        free(m_userdata);
        m_userdata = nullptr;
    }
}

void InstructionImpl::setMnemonic(const char *s) { m_mnemonic = s; }
void InstructionImpl::setAddress(address_t address) { m_address = address; }
void InstructionImpl::setSize(u32 size) { m_size = size; }
void InstructionImpl::setId(instruction_id_t id) { m_id = id; }
void InstructionImpl::setType(InstructionType type) { m_type = type; }
void InstructionImpl::setFree(const Callback_VoidPointer &cb) { m_free = cb; }
void InstructionImpl::setUserData(void *userdata) { m_userdata = userdata; }

} // namespace REDasm
