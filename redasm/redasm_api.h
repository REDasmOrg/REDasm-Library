#ifndef REDASM_API_H
#define REDASM_API_H

#include <memory>
#include <functional>
#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <map>
#include <list>
#include <set>
#include "types/base_types.h"
#include "types/buffer/abstractbuffer.h"
#include "types/buffer/bufferview.h"
#include "support/utils.h"
#include "redasm_context.h"

#if __cplusplus <= 201103L && __GNUC__
namespace std {
template<typename T, typename... Args> std::unique_ptr<T> make_unique(Args&&... args) { return std::unique_ptr<T>(new T(std::forward<Args>(args)...)); }
}
#endif

#define DO_UNPAREN(...) __VA_ARGS__
#define INVOKE(expr)    expr
#define UNPAREN(args)   INVOKE(DO_UNPAREN args)

#define RE_UNUSED(x)                               (void)x
#define ENTRYPOINT_FUNCTION                        "__redasm_ep__"
#define START_FUNCTION                             "__redasm_start__"
#define REGISTER_INVALID                           static_cast<s64>(-1)
#define BRANCH_DIRECTION(instruction, destination) (static_cast<s64>(destination) - static_cast<s64>(instruction->address))

namespace REDasm {

inline void log(const std::string& s) { Context::settings.logCallback(s); }

inline void status(const std::string& s) {
    CONTEXT_DEBOUNCE_CHECK
    Context::settings.statusCallback(s);
}

inline void statusProgress(const std::string& s, size_t p) {
    CONTEXT_DEBOUNCE_CHECK
    Context::settings.statusCallback(s);
    Context::settings.progressCallback(p);
}

inline void statusAddress(const std::string& s, address_t address) {
    CONTEXT_DEBOUNCE_CHECK
    Context::settings.statusCallback(s + " @ " + REDasm::hex(address));
}

template<typename... T> std::string makePath(const std::string& p, T... args) {
    std::string path = p;
    std::deque<std::string> v = { args... };

    for(size_t i = 0; i < v.size(); i++)
    {
        if(!path.empty() && (path.back() != Context::dirSeparator[0]))
            path += Context::dirSeparator;

        path += v[i];
    }

    return path;
}

template<typename...T> std::string makeRntPath(const std::string& p, T... args) { return REDasm::makePath(Context::settings.searchPath, p, args...); }
template<typename...T> std::string makeDbPath(const std::string& p, T... args) { return REDasm::makeRntPath("database", p, args...); }
template<typename...T> std::string makeSdbPath(const std::string& p, T... args) { return REDasm::makeDbPath("sdb", p, args...); }
template<typename...T> std::string makeLoaderPath(const std::string& p, T... args) { return REDasm::makeDbPath("loaders", p, args...); }
template<typename...T> std::string makeSignaturePath(const std::string& p, T... args) { return REDasm::makeDbPath("signatures", p, args...); }

namespace SegmentTypes {
    enum: u32 {
        None = 0x00000000,
        Code = 0x00000001,
        Data = 0x00000002,
        Bss  = 0x00000004,

        PureCode = Code & (~Data | ~Bss),
    };
}

namespace InstructionTypes {
    enum: u32 {
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
}

namespace OperandTypes {
    enum: u32 {
        None          = 0x00000000,
        Constant      = 0x00000001,  // Simple constant
        Register      = 0x00000002,  // Register
        Immediate     = 0x00000004,  // Immediate Value
        Memory        = 0x00000008,  // Direct Memory Pointer
        Displacement  = 0x00000010,  // Indirect Memory Pointer

        Local         = 0x00010000,  // Local Variable
        Argument      = 0x00020000,  // Function Argument
    };
}

namespace OperandSizes {
    enum: u32 {
        Undefined  = 0,
        Byte       = 1,
        Word       = 2,
        Dword      = 4,
        Qword      = 8,
    };

    std::string size(u32 opsize);
}

struct Segment
{
    Segment(): offset(0), address(0), endaddress(0), type(0) { }
    Segment(const std::string& name, offset_t offset, address_t address, u64 psize, u64 vsize, u64 type): name(name), offset(offset), endoffset(offset + psize), address(address), endaddress(address + vsize), type(type) { }
    constexpr s64 size() const { return static_cast<s64>(endaddress - address); }
    constexpr s64 rawSize() const { return static_cast<s64>(endoffset - offset); }
    constexpr bool empty() const { return this->size() <= 0; }
    constexpr bool contains(address_t address) const { return (address >= this->address) && (address < endaddress); }
    constexpr bool containsOffset(offset_t offset) const { return !is(SegmentTypes::Bss) && ((offset >= this->offset) && (offset < this->endoffset)); }
    constexpr bool is(u32 t) const { return type & t; }

    std::string name;
    offset_t offset, endoffset;
    address_t address, endaddress;
    u64 type;
};

struct RegisterOperand
{
    RegisterOperand(): tag(0), r(REGISTER_INVALID) { }
    RegisterOperand(u64 type, register_id_t r): tag(type), r(r) { }
    RegisterOperand(register_id_t r): tag(0), r(r) { }

    u64 tag;
    register_id_t r;

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
    Operand(): loc_index(-1), type(OperandTypes::None), tag(0), size(OperandSizes::Undefined), index(-1), u_value(0) { }
    Operand(u32 type, u32 tag, s32 value, s64 idx): loc_index(-1), type(type), tag(tag), size(OperandSizes::Undefined), index(idx), s_value(value) { }
    Operand(u32 type, u32 tag, u32 value, s64 idx): loc_index(-1), type(type), tag(tag), size(OperandSizes::Undefined), index(idx), u_value(value) { }
    Operand(u32 type, u32 tag, s64 value, s64 idx): loc_index(-1), type(type), tag(tag), size(OperandSizes::Undefined), index(idx), s_value(value) { }
    Operand(u32 type, u32 tag, u64 value, s64 idx): loc_index(-1), type(type), tag(tag), size(OperandSizes::Undefined), index(idx), u_value(value) { }

    s64 loc_index;
    u32 type, tag, size;
    s64 index;
    RegisterOperand reg;
    DisplacementOperand disp;

    union { s64 s_value; u64 u_value; };

    constexpr bool displacementIsDynamic() const { return is(OperandTypes::Displacement) && (disp.base.isValid() || disp.index.isValid()); }
    constexpr bool displacementCanBeAddress() const { return is(OperandTypes::Displacement) && (disp.displacement > 0); }
    constexpr bool isNumeric() const { return is(OperandTypes::Constant) || is(OperandTypes::Immediate) || is(OperandTypes::Memory); }
    constexpr bool is(u32 t) const { return type & t; }
};

struct Instruction
{
    Instruction(): address(0), target_idx(-1), type(0), size(0), id(0), userdata(nullptr) { }
    ~Instruction() { reset(); }

    std::function<void(void*)> free;

    std::string mnemonic;
    std::set<address_t> targets;    // Jump/JumpTable/Call destination(s)
    std::deque<Operand> operands;
    address_t address;
    s32 target_idx;                 // Target's operand index
    u32 type, size;
    instruction_id_t id;            // Backend Specific
    void* userdata;                 // It doesn't survive after AssemblerPlugin::decode() by design

    constexpr bool is(u32 t) const { return type & t; }
    constexpr bool isTargetOperand(const Operand* op) const { return (target_idx == -1) ? false : (target_idx == op->index); }
    constexpr bool isInvalid() const { return type == InstructionTypes::Invalid; }
    inline bool hasTargets() const { return !targets.empty(); }
    inline void target(address_t target) { targets.insert(target); }
    inline void untarget(address_t target) { targets.erase(target); }
    inline void op_size(s32 index, u32 size) { operands[index].size = size; }
    inline u32 op_size(s32 index) const { return operands[index].size; }
    inline address_t target() const { return *targets.begin(); }
    constexpr address_t endAddress() const { return address + size; }

    inline Operand* targetOperand() { return &operands[target_idx]; }
    inline Operand* op(size_t idx = 0) { return (idx < operands.size()) ? &operands[idx] : nullptr; }
    inline Instruction& mem(address_t v, u32 tag = 0) { operands.emplace_back(OperandTypes::Memory, tag, v, operands.size()); return *this; }
    template<typename T> Instruction& cnst(T v, u32 tag = 0) { operands.emplace_back(OperandTypes::Constant, tag, v, operands.size()); return *this; }
    template<typename T> Instruction& imm(T v, u32 tag = 0) { operands.emplace_back(OperandTypes::Immediate, tag, v, operands.size()); return *this; }
    template<typename T> Instruction& disp(register_id_t base, T displacement = 0) { return disp(base, REGISTER_INVALID, displacement); }
    template<typename T> Instruction& disp(register_id_t base, register_id_t index, T displacement) { return disp(base, index, 1, displacement); }
    template<typename T> Instruction& disp(register_id_t base, register_id_t index, s64 scale, T displacement);
    template<typename T> Instruction& arg(s64 locindex, register_id_t base, register_id_t index, T displacement) { return local(locindex, base, index, displacement, OperandTypes::Argument); }
    template<typename T> Instruction& local(s64 locindex, register_id_t base, register_id_t index, T displacement, u32 type = OperandTypes::Local);

    void targetOp(s32 index) {
        target_idx = index;

        if((index < operands.size()) && operands[index].isNumeric())
            this->target(operands[index].u_value);
    }

    Instruction& reg(register_id_t r, u64 type = 0) {
        Operand op;
        op.index = operands.size();
        op.type = OperandTypes::Register;
        op.reg = RegisterOperand(type, r);

        operands.emplace_back(op);
        return *this;
    }

    void reset() {
        target_idx = -1;
        type = size = 0;

        targets.clear();
        operands.clear();

        if(free && userdata) {
            free(userdata);
            userdata = nullptr;
        }
    }
};

template<typename T> Instruction& Instruction::disp(register_id_t base, register_id_t index, s64 scale, T displacement)
{
    Operand op;
    op.index = operands.size();

    if((base == REGISTER_INVALID) && (index == REGISTER_INVALID))
    {
        op.type = OperandTypes::Memory;
        op.u_value = scale * displacement;
    }
    else
    {
        op.type = OperandTypes::Displacement;
        op.disp = DisplacementOperand(RegisterOperand(base), RegisterOperand(index), scale, displacement);
    }

    operands.emplace_back(op);
    return *this;
}

template<typename T> Instruction& Instruction::local(s64 locindex, register_id_t base, register_id_t index, T displacement, u32 type)
{
    Operand op;
    op.index = operands.size();
    op.loc_index = locindex;
    op.type = OperandTypes::Displacement | type;
    op.disp = DisplacementOperand(RegisterOperand(base), RegisterOperand(index), 1, displacement);

    operands.emplace_back(op);
    return *this;
}

typedef std::shared_ptr<Instruction> InstructionPtr;
typedef std::deque<Operand> OperandList;
typedef std::deque<Segment> SegmentList;
typedef std::list<std::string> SignatureFiles;

} // namespace REDasm

#endif // REDASM_API_H
