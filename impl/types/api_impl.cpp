#include "api_impl.h"

namespace REDasm {

InstructionImpl::InstructionImpl(Instruction *q): m_pimpl_q(q), m_free(nullptr), m_userdata(nullptr) { }
InstructionImpl::~InstructionImpl() { this->reset(); }
bool InstructionImpl::hasOperands() const { return !m_operands.empty(); }
size_t InstructionImpl::operandsCount() const { return m_operands.size(); }
const Operand *InstructionImpl::op(size_t idx) const { return variant_object<Operand>(m_operands.at(idx)); }
Operand *InstructionImpl::op(size_t idx) { return variant_object<Operand>(m_operands.at(idx)); }

const Operand *InstructionImpl::target() const
{
    size_t index = m_operands.find([](const Variant& v) -> bool {
        const Operand* op = variant_object<Operand>(v);
        return op->isTarget();
    });

    return index != REDasm::npos ? variant_object<Operand>(m_operands[index]) : nullptr;
}

Instruction *InstructionImpl::mem(address_t v, tag_t tag)
{
    PIMPL_Q(Instruction);
    m_operands.append(new Operand(OperandType::Memory, v, m_operands.size(), tag));
    return q;
}

Instruction *InstructionImpl::cnst(u64 v, tag_t tag)
{
    PIMPL_Q(Instruction);
    m_operands.append(new Operand(OperandType::Constant, v, m_operands.size(), tag));
    return q;
}

Instruction *InstructionImpl::imm(u64 v, tag_t tag)
{
    PIMPL_Q(Instruction);
    m_operands.append(new Operand(OperandType::Immediate, v, m_operands.size(), tag));
    return q;
}

Instruction *InstructionImpl::disp(register_id_t base, s64 displacement) { return this->disp(base, REDasm::npos, displacement); }
Instruction *InstructionImpl::disp(register_id_t base, register_id_t index, s64 displacement) { return this->disp(base, index, 1, displacement);  }

Instruction *InstructionImpl::disp(register_id_t base, register_id_t index, s64 scale, s64 displacement)
{
    PIMPL_Q(Instruction);

    Operand* op = new Operand();
    op->index = m_operands.size();

    if((base == REDasm::npos) && (index == REDasm::npos))
    {
        op->type = OperandType::Memory;
        op->u_value = scale * displacement;
    }
    else
    {
        op->type = OperandType::Displacement;
        op->disp = DisplacementOperand(RegisterOperand(base), RegisterOperand(index), scale, displacement);
    }

    m_operands.append(op);
    return q;
}

Instruction *InstructionImpl::arg(size_t locindex, register_id_t base, register_id_t index, s64 displacement) { return this->local(locindex, base, index, displacement, OperandFlags::Argument); }

Instruction *InstructionImpl::local(size_t locindex, register_id_t base, register_id_t index, s64 displacement, OperandFlags flags)
{
    PIMPL_Q(Instruction);

    Operand* op = new Operand();
    op->index = m_operands.size();
    op->loc_index = locindex;
    op->type = OperandType::Displacement;
    op->flags = flags;
    op->disp = DisplacementOperand(RegisterOperand(base), RegisterOperand(index), 1, displacement);

    m_operands.append(op);
    return q;
}

Instruction *InstructionImpl::reg(register_id_t r, tag_t tag)
{
    PIMPL_Q(Instruction);

    Operand* op = new Operand();
    op->index = m_operands.size();
    op->type = OperandType::Register;
    op->reg = RegisterOperand(r, tag);

    m_operands.append(op);
    return q;
}

Instruction *InstructionImpl::tgt(address_t a)
{
    PIMPL_Q(Instruction);

    this->imm(a, 0);

    Operand* op = variant_object<Operand>(m_operands.last());
    op->asTarget();

    this->target(a);
    return q;
}

const List &InstructionImpl::targets() const { return m_targets; }

void InstructionImpl::target(address_t address)
{
    auto it = m_utargets.find(address);

    if(it != m_utargets.end())
        return;

    m_targets.append(address);
}

void InstructionImpl::targetIdx(size_t idx)
{
    PIMPL_Q(Instruction);

    if(idx >= m_operands.size())
        return;

    Operand* op = this->op(idx);
    op->asTarget();

    if(op->isNumeric())
        this->target(op->u_value);
}

void *InstructionImpl::userData() const { return m_userdata; }

void InstructionImpl::reset()
{
    PIMPL_Q(Instruction);

    q->type = InstructionType::None;
    q->size = 0;
    m_operands.releaseObjects();

    if(m_free && m_userdata)
    {
        free(m_userdata);
        m_userdata = nullptr;
    }
}

void InstructionImpl::setFree(const Callback_VoidPointer &cb) { m_free = cb; }
void InstructionImpl::setUserData(void *userdata) { m_userdata = userdata; }

} // namespace REDasm
