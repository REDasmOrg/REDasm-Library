#pragma once

#include "../string.h"
#include "../callback.h"
#include "api_base.h"
#include "operand.h"

namespace REDasm {

DECLARE_TYPES(Instruction, None, Invalid, Stop, Nop, Jump, Call, Add, Sub, Mul, Div,
              Mod, Lsh, Rsh, And, Or, Xor, Not, Push, Pop, Compare,
              Load, Store)

namespace InstructionFlags {
enum: flag_t {
    None        = (1 << 0),
    Weak        = (1 << 1),
    Conditional = (1 << 2),
    Privileged  = (1 << 3),
};
}

struct Instruction
{
    instruction_id_t id;            // Implementation Specific

    union {                         // Implementation Specific
        uintptr_t userdata;
        void* puserdata{nullptr};
    };

    Callback_UIntPtr free{nullptr}; // Implementation Specific

    type_t type{InstructionType::None};
    flag_t flags{InstructionFlags::None};
    address_t address;
    u32 size;
    char mnemonic_[DEFAULT_NAME_SIZE];

    size_t targetscount;
    address_t targets[DEFAULT_CONTAINER_SIZE];

    size_t operandscount;
    Operand operandsstruct[DEFAULT_CONTAINER_SIZE];

    Instruction() = default;
    ~Instruction();
    String mnemonic() const;
    void mnemonic(const String& s);
    address_t endAddress() const;
    bool hasOperands() const;
    bool contains(address_t addr) const;

    const Operand* op(size_t idx) const;
    const Operand* firstOperand() const;
    const Operand* lastOperand() const;
    const Operand* target() const;
    Operand* op(size_t idx);
    Operand* firstOperand();
    Operand* lastOperand();
    Operand* target();

    Instruction* mem(address_t v, tag_t tag = 0);
    Instruction* cnst(u64 v, tag_t tag = 0);
    Instruction* imm(u64 v, tag_t tag = 0);
    Instruction* disp(register_id_t base, s64 displacement);
    Instruction* disp(register_id_t base, register_id_t index, s64 displacement);
    Instruction* disp(register_id_t base, register_id_t index, s64 scale, s64 displacement);
    Instruction* arg(size_t locindex, register_id_t base, register_id_t index, s64 displacement);
    Instruction* local(size_t locindex, register_id_t base, register_id_t index, s64 displacement, flag_t opflags = OperandFlags::None);
    Instruction* reg(register_id_t r, tag_t tag = 0);
    Instruction* tgt(address_t addr);
    Instruction* op(type_t optype, s64 val, tag_t t = 0);
    Instruction* op(type_t optype, u64 val, tag_t t = 0);
    Instruction* op(const Operand& op);

    Instruction* target(address_t addr);
    Instruction* targetIdx(size_t idx);

    bool isInvalid() const;
    bool is(const String& mnemonic) const;
    bool typeIs(type_t t) const;
    bool isStop() const;
    bool isCall() const;
    bool isJump() const;
    bool isConditionalCall() const;
    bool isConditionalJump() const;
    bool isBranch() const;
    bool isConditional() const;

    void reset();
    operator bool() const;
};

FORCE_STANDARD_LAYOUT(Instruction)

} // namespace REDasm
