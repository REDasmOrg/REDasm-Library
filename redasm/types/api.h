#pragma once

#include "../macros.h"
#include "../pimpl.h"
#include "containers/list.h"
#include "base.h"
#include "string.h"
#include "callback.h"

namespace REDasm {

enum class SegmentType: u32 {
    None = 0x00000000,
    Code = 0x00000001,
    Data = 0x00000002,
    Bss  = 0x00000004,
};

ENUM_FLAGS_OPERATORS(SegmentType)

enum class InstructionType: u32 {
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

enum class OperandType : u32 {
    None          = 0x00000000,
    Constant      = 0x00000001,  // Simple constant
    Register      = 0x00000002,  // Register
    Immediate     = 0x00000004,  // Immediate Value
    Memory        = 0x00000008,  // Direct Memory Pointer
    Displacement  = 0x00000010,  // Indirect Memory Pointer

    Local         = 0x00010000,  // Local Variable
    Argument      = 0x00020000,  // Function Argument
    Target        = 0x00040000,  // Branch destination
};

ENUM_FLAGS_OPERATORS(OperandType)

class Segment: public Object
{
    REDASM_FACTORY_OBJECT(Segment)

    public:
        Segment();
        Segment(const String& name, offset_t offset, address_t address, u64 psize, u64 vsize, SegmentType type);
        u64 size() const;
        u64 rawSize() const;
        bool empty() const;
        bool contains(address_t address) const;
        bool containsOffset(offset_t offset) const;
        bool is(SegmentType t) const;
        bool isPureCode() const;

    public:
        String name;
        offset_t offset, endoffset;
        address_t address, endaddress;
        SegmentType type;
};

struct RegisterOperand
{
    RegisterOperand();
    RegisterOperand(register_id_t r, tag_t tag);
    RegisterOperand(register_id_t r);
    bool isValid() const;
    void save(cereal::BinaryOutputArchive& a) const;
    void load(cereal::BinaryInputArchive& a);

    register_id_t r;
    tag_t tag;
};

struct DisplacementOperand
{
    DisplacementOperand();
    DisplacementOperand(const RegisterOperand& base, const RegisterOperand& index, s64 scale, s64 displacement);
    void save(cereal::BinaryOutputArchive& a) const;
    void load(cereal::BinaryInputArchive& a);

    RegisterOperand base, index;
    s64 scale;
    s64 displacement;
};

class Operand: public Object
{
    REDASM_FACTORY_OBJECT(Operand)

    public:
        Operand();
        Operand(OperandType type, s64 value, size_t idx, tag_t tag);
        Operand(OperandType type, u64 value, size_t idx, tag_t tag);
        bool displacementIsDynamic() const;
        bool displacementCanBeAddress() const;
        bool isCharacter() const;
        bool isNumeric() const;
        bool isTarget() const;
        bool is(OperandType t) const;
        void asTarget();
        bool checkCharacter();
        void save(cereal::BinaryOutputArchive& a) const override;
        void load(cereal::BinaryInputArchive& a) override;

    public:
        OperandType type;
        tag_t tag;
        u32 size;
        size_t index, loc_index;
        RegisterOperand reg;
        DisplacementOperand disp;
        union { s64 s_value; u64 u_value; };
};

class InstructionImpl;

class Instruction: public Object
{
    REDASM_FACTORY_OBJECT(Instruction)
    PIMPL_DECLARE_P(Instruction)
    PIMPL_DECLARE_PRIVATE(Instruction)

    public:
        Instruction();
        address_t endAddress() const;
        bool hasOperands() const;
        size_t operandsCount() const;
        ListConstIterator iterator() const;
        const Operand* op(size_t idx) const;
        const Operand* firstOperand() const;
        const Operand* lastOperand() const;
        Operand* op(size_t idx);
        Operand* firstOperand();
        Operand* lastOperand();
        const Operand *target() const;
        Instruction* mem(address_t v, tag_t tag = 0);
        Instruction* cnst(u64 v, tag_t tag = 0);
        Instruction* imm(u64 v, tag_t tag = 0);
        Instruction* disp(register_id_t base, s64 displacement = 0);
        Instruction* disp(register_id_t base, register_id_t index, s64 displacement);
        Instruction* disp(register_id_t base, register_id_t index, s64 scale, s64 displacement);
        Instruction* arg(size_t locindex, register_id_t base, register_id_t index, s64 displacement);
        Instruction* local(size_t locindex, register_id_t base, register_id_t index, s64 displacement, OperandType type = OperandType::Local);
        Instruction* reg(register_id_t r, tag_t tag = 0);

    public:
        const List& targets() const;
        void target(address_t address);
        void targetIdx(size_t idx);

    public:
        bool is(InstructionType t) const;
        bool is(const char* s) const;
        bool isInvalid() const;
        void* userData() const;
        void reset();

    public:
        void setFree(const Callback_VoidPointer& cb);
        void setUserData(void* userdata);
        void save(cereal::BinaryOutputArchive& a) const override;
        void load(cereal::BinaryInputArchive& a) override;

    public:
        instruction_id_t id; // Backend Specific
        InstructionType type;
        String mnemonic;
        address_t address;
        u32 size;
};

} // namespace REDasm
