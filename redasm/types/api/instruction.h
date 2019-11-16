#pragma once

#include "../string.h"
#include "../callback.h"
#include "api_base.h"
#include "operand.h"

namespace REDasm {

enum class InstructionType: type_t {
    None            = 0x00000000, Stop = 0x00000001, Nop = 0x00000002,
    Jump            = 0x00000004, Call = 0x00000008,
    Add             = 0x00000010, Sub  = 0x00000020, Mul = 0x00000040, Div = 0x0000080, Mod = 0x00000100, Lsh = 0x00000200, Rsh = 0x00000400,
    And             = 0x00000800, Or   = 0x00001000, Xor = 0x00002000, Not = 0x0004000,
    Push            = 0x00008000, Pop  = 0x00010000,
    Compare         = 0x00020000, Load = 0x00040000, Store = 0x00080000,

    Conditional     = 0x01000000, Privileged = 0x02000000,
    Invalid         = 0x10000000,
    Branch          = Jump | Call,
    ConditionalJump = Conditional | Jump,
    ConditionalCall = Conditional | Call,
};

ENUM_FLAGS_OPERATORS(InstructionType)

enum class InstructionFlags: flag_t
{
    None = (1 << 0),
    Weak = (1 << 1)
};

ENUM_FLAGS_OPERATORS(InstructionFlags)

struct Instruction
{
    instruction_id_t id;            // Implementation Specific

    union {                         // Implementation Specific
        uintptr_t userdata;
        void* puserdata{nullptr};
    };

    Callback_UIntPtr free{nullptr}; // Implementation Specific

    InstructionType type{InstructionType::None};
    InstructionFlags flags{InstructionFlags::None};
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
    Instruction* local(size_t locindex, register_id_t base, register_id_t index, s64 displacement, OperandFlags opflags = OperandFlags::None);
    Instruction* reg(register_id_t r, tag_t tag = 0);
    Instruction* tgt(address_t addr);
    Instruction* op(OperandType optype, s64 val, tag_t t = 0);
    Instruction* op(OperandType optype, u64 val, tag_t t = 0);
    Instruction* op(const Operand& op);

    Instruction* target(address_t addr);
    Instruction* targetIdx(size_t idx);

    void reset();
    bool isInvalid() const;
    bool is(const String& mnemonic) const;
    bool typeIs(InstructionType t) const;
    operator bool() const;
};

FORCE_STANDARD_LAYOUT(Instruction)

} // namespace REDasm
