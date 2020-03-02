#pragma once

#include "../string.h"
#include "../callback.h"
#include "api_base.h"
#include "operand.h"

namespace REDasm {

struct Instruction
{
    enum Type: type_t {
        T_None, T_Invalid, T_Stop, T_Nop, T_Jump, T_Call, T_Add, T_Sub, T_Mul, T_Div,
        T_Mod, T_Lsh, T_Rsh, T_And, T_Or, T_Xor, T_Not, T_Push, T_Pop, T_Compare,
        T_Load, T_Store
    };

    enum Flags: flag_t {
        F_None,
        F_Weak        = (1 << 1),
        F_Conditional = (1 << 2),
        F_Privileged  = (1 << 3),
    };

    instruction_id_t id;            // Implementation Specific

    union {                         // Implementation Specific
        uintptr_t userdata;
        void* puserdata{nullptr};
    };

    Callback_UIntPtr free{nullptr}; // Implementation Specific

    type_t type{Instruction::T_None};
    flag_t flags{Instruction::F_None};
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
    Instruction* local(size_t locindex, register_id_t base, register_id_t index, s64 displacement, flag_t opflags = Operand::F_None);
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
