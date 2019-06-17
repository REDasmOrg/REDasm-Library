#pragma once

#include <memory>
#include <deque>
#include <set>
#include "../support/serializer.h"
#include "../macros.h"
#include "../pimpl.h"
#include "base_types.h"
#include "callback_types.h"

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

struct Segment
{
    Segment(): offset(0), address(0), endaddress(0), type(SegmentType::None) { }
    Segment(const std::string& name, offset_t offset, address_t address, u64 psize, u64 vsize, SegmentType type): name(name), offset(offset), endoffset(offset + psize), address(address), endaddress(address + vsize), type(type) { }
    constexpr s64 size() const { return static_cast<s64>(endaddress - address); }
    constexpr s64 rawSize() const { return static_cast<s64>(endoffset - offset); }
    constexpr bool empty() const { return this->size() <= 0; }
    constexpr bool contains(address_t address) const { return (address >= this->address) && (address < endaddress); }
    constexpr bool containsOffset(offset_t offset) const { return !is(SegmentType::Bss) && ((offset >= this->offset) && (offset < this->endoffset)); }
    constexpr bool is(SegmentType t) const { return type & t; }
    constexpr bool isPureCode() const { return type == SegmentType::Code; }

    std::string name;
    offset_t offset, endoffset;
    address_t address, endaddress;
    SegmentType type;
};

struct RegisterOperand
{
    RegisterOperand(): r(REGISTER_INVALID), tag(0) { }
    RegisterOperand(register_id_t r, tag_t tag): r(r), tag(tag) { }
    RegisterOperand(register_id_t r): r(r), tag(0) { }

    register_id_t r;
    tag_t tag;

    bool isValid() const { return r != REGISTER_INVALID; }
};

struct DisplacementOperand
{
    DisplacementOperand(): scale(1), displacement(0) { }
    DisplacementOperand(const RegisterOperand& base, const RegisterOperand& index, s64 scale, s64 displacement): base(base), index(index), scale(scale), displacement(displacement) { }

    RegisterOperand base, index;
    s64 scale;
    s64 displacement;
};

struct Operand
{
    Operand(): type(OperandType::None), tag(0), size(0), index(REDasm::npos), loc_index(REDasm::npos), u_value(0) { }
    Operand(OperandType type, s32 value, size_t idx, tag_t tag): type(type), tag(tag), size(0), index(idx), loc_index(REDasm::npos), s_value(value) { }
    Operand(OperandType type, u32 value, size_t idx, tag_t tag): type(type), tag(tag), size(0), index(idx), loc_index(REDasm::npos), u_value(value) { }
    Operand(OperandType type, s64 value, size_t idx, tag_t tag): type(type), tag(tag), size(0), index(idx), loc_index(REDasm::npos), s_value(value) { }
    Operand(OperandType type, u64 value, size_t idx, tag_t tag): type(type), tag(tag), size(0), index(idx), loc_index(REDasm::npos), u_value(value) { }

    OperandType type;
    tag_t tag;
    u32 size;
    size_t index, loc_index;
    RegisterOperand reg;
    DisplacementOperand disp;
    union { s64 s_value; u64 u_value; };

    constexpr bool displacementIsDynamic() const { return is(OperandType::Displacement) && (disp.base.isValid() || disp.index.isValid()); }
    constexpr bool displacementCanBeAddress() const { return is(OperandType::Displacement) && (disp.displacement > 0); }
    constexpr bool isCharacter() const { return is(OperandType::Constant) && (u_value <= 0xFF) && ::isprint(static_cast<u8>(u_value)); }
    constexpr bool isNumeric() const { return is(OperandType::Constant) || is(OperandType::Immediate) || is(OperandType::Memory); }
    constexpr bool isTarget() const { return type & OperandType::Target; }
    constexpr bool is(OperandType t) const { return type & t; }
    void asTarget() { type |= OperandType::Target; }

    bool checkCharacter() {
        if(!is(OperandType::Immediate) || (u_value > 0xFF) || !::isprint(static_cast<u8>(u_value)))
            return false;

        type = OperandType::Constant;
        return true;
    }
};


class InstructionImpl;

class Instruction
{
    PIMPL_DECLARE_P(Instruction)
    PIMPL_DECLARE_PRIVATE(Instruction)

    public:
        Instruction();
        const char* mnemonic() const;
        address_t address() const;
        address_t endAddress() const;
        u32 size() const;
        instruction_id_t id() const;
        const InstructionType& type() const;
        InstructionType& type();

    public:
        bool hasOperands() const;
        size_t operandsCount() const;
        size_t opSize(size_t idx) const;
        const Operand* op(size_t idx) const;
        Operand* op(size_t idx);
        const Operand *target() const;
        void opSize(size_t idx, size_t size);
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
        size_t targetsCount() const;
        address_t targetAt(size_t idx) const;
        void target(address_t address);
        void targetIdx(size_t idx);
        void clearTargets();

    public:
        bool is(InstructionType t) const;
        bool isInvalid() const;
        void* userData() const;
        void reset();

    public:
        void setMnemonic(const char* s);
        void setAddress(address_t address);
        void setSize(u32 size);
        void setId(instruction_id_t id);
        void setType(InstructionType type);
        void setFree(const Callback_VoidPointer& cb);
        void setUserData(void* userdata);

    friend struct Serializer<Instruction>;
};

template<> struct Serializer<Instruction> {
    static void write(std::fstream& fs, const Instruction& st);
    static void read(std::fstream& fs, Instruction& st);
};

typedef std::shared_ptr<Instruction> InstructionPtr;
typedef std::deque<Operand> OperandList;
typedef std::deque<Segment> SegmentList;

} // namespace REDasm

VISITABLE_STRUCT(REDasm::RegisterOperand, r, tag);
VISITABLE_STRUCT(REDasm::DisplacementOperand, base, index, scale, displacement);
VISITABLE_STRUCT(REDasm::Operand, type, tag, size, index, loc_index, reg, disp, u_value);
VISITABLE_STRUCT(REDasm::Segment, name, offset, endoffset, address, endaddress, type);
