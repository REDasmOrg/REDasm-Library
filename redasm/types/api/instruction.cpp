#include "instruction.h"
#include <algorithm>

namespace REDasm {

Instruction::~Instruction() { this->reset(); }
String Instruction::mnemonic() const { return this->mnemonic_; }
void Instruction::mnemonic(const String& s) { s.copy(this->mnemonic_, DEFAULT_NAME_SIZE); }
address_t Instruction::endAddress() const { return this->address + this->size; }
bool Instruction::hasOperands() const { return this->operandscount > 0; }
bool Instruction::contains(address_t addr) const { return (addr >= this->address) && (addr < this->endAddress()); }
const Operand* Instruction::op(size_t idx) const { return const_cast<Instruction*>(this)->op(idx); }
const Operand* Instruction::firstOperand() const { return const_cast<Instruction*>(this)->firstOperand(); }
const Operand* Instruction::lastOperand() const { return const_cast<Instruction*>(this)->lastOperand(); }
const Operand* Instruction::target() const { return const_cast<Instruction*>(this)->target(); }

Operand* Instruction::op(size_t idx)
{
    if(idx >= operandscount)
        throw std::out_of_range("Operand out of range");

    return &operandsstruct[idx];
}

Operand* Instruction::firstOperand() { return operandscount ? this->op(0) : nullptr; }
Operand* Instruction::lastOperand() { return operandscount ? this->op(operandscount - 1) : nullptr; }

Operand* Instruction::target()
{
    auto it = std::find_if(std::begin(operandsstruct), std::end(operandsstruct), [](const Operand& op) -> bool{
        return op.flags & OperandFlags::Target;
    });

    return it != std::end(operandsstruct) ? std::addressof(*it) : nullptr;
}

Instruction* Instruction::mem(address_t v, tag_t tag) { return this->op(OperandType::Memory, v, tag); }
Instruction* Instruction::cnst(address_t v, tag_t tag) { return this->op(OperandType::Constant, v, tag); }
Instruction* Instruction::imm(u64 v, tag_t tag) { return this->op(OperandType::Immediate, v, tag); }
Instruction* Instruction::disp(register_id_t base, s64 displacement) { return this->disp(base, REDasm::npos, displacement); }
Instruction* Instruction::disp(register_id_t base, register_id_t index, s64 displacement) { return this->disp(base, index, 1, displacement); }

Instruction* Instruction::disp(register_id_t base, register_id_t index, s64 scale, s64 displacement)
{
    Operand op{ };

    if((base == REDasm::npos) && (index == REDasm::npos))
    {
        op.type = OperandType::Memory;
        op.u_value = scale * displacement;
    }
    else
    {
        op.type = OperandType::Displacement;
        op.disp.basestruct.r = base;
        op.disp.indexstruct.r = index;
        op.disp.scale = scale;
        op.disp.displacement = displacement;
    }

    return this->op(op);
}

Instruction* Instruction::arg(size_t locindex, register_id_t base, register_id_t index, s64 displacement) { return this->local(locindex, base, index, displacement, OperandFlags::Argument); }

Instruction* Instruction::local(size_t locindex, register_id_t base, register_id_t index, s64 displacement, flag_t opflags)
{
    Operand op;
    op.index = this->operandscount;
    op.loc_index = locindex;
    op.type = OperandType::Displacement;
    op.flags = opflags;
    op.disp.basestruct.r = base;
    op.disp.indexstruct.r = index;
    op.disp.scale = 1;
    op.disp.displacement = displacement;
    return this->op(op);
}

Instruction* Instruction::reg(register_id_t r, tag_t tag)
{
    Operand op;
    op.index = this->operandscount;
    op.type = OperandType::Register;
    op.reg.r = r;
    op.reg.tag = tag;
    return this->op(op);
}

Instruction* Instruction::tgt(address_t addr)
{
    this->imm(addr, 0);
    operandsstruct[operandscount - 1].asTarget();
    return this->target(addr);
}

Instruction* Instruction::op(type_t optype, s64 val, tag_t t)
{
    Operand op{ };
    op.type = optype;
    op.s_value = val;
    op.tag = t;

    return this->op(op);
}

Instruction* Instruction::op(type_t optype, u64 val, tag_t t)
{
    Operand op{ };
    op.type = optype;
    op.u_value = val;
    op.tag = t;

    return this->op(op);
}

Instruction* Instruction::op(const Operand& op)
{
    if(operandscount >= DEFAULT_CONTAINER_SIZE)
        throw std::out_of_range("Too many operands");

    size_t idx = operandscount;
    operandsstruct[operandscount++] = op;
    operandsstruct[idx].index = idx;
    return this;
}

Instruction* Instruction::target(address_t addr)
{
    if(targetscount >= DEFAULT_CONTAINER_SIZE)
        throw std::out_of_range("Too many targets");

    auto it = std::find(std::begin(targets), std::end(targets), addr);

    if(it == std::end(targets))
        targets[targetscount++] = addr;

    return this;
}

Instruction* Instruction::targetIdx(size_t idx)
{
    if(idx >= DEFAULT_CONTAINER_SIZE) return this;

    auto& op = operandsstruct[idx];
    op.asTarget();

    if(op.isNumeric())
        this->target(op.u_value);

    return this;
}

void Instruction::reset()
{
    if(this->free && this->userdata)
        this->free(this->userdata);

    this->free = nullptr;
    this->userdata = { };
    this->type = InstructionType::Invalid;
}

bool Instruction::isInvalid() const { return type == InstructionType::Invalid; }
bool Instruction::is(const String& mnemonic) const { return this->mnemonic() == mnemonic; }
bool Instruction::typeIs(type_t t) const { return REDasm::typeIs(this, t); }
bool Instruction::isStop() const { return REDasm::typeIs(this, InstructionType::Stop); }
bool Instruction::isCall() const { return REDasm::typeIs(this, InstructionType::Call); }
bool Instruction::isJump() const { return REDasm::typeIs(this, InstructionType::Jump); }
bool Instruction::isConditionalCall() const { return this->isCall() && this->isConditional(); }
bool Instruction::isConditionalJump() const { return this->isJump() && this->isConditional(); }
bool Instruction::isBranch() const { return this->isCall() || this->isJump(); }
bool Instruction::isConditional() const { return REDasm::hasFlag(this, InstructionFlags::Conditional); }
Instruction::operator bool() const { return type != InstructionType::Invalid; }

} // namespace REDasm

