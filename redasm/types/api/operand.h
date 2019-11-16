#pragma once

#include "api_base.h"

namespace REDasm {

enum class OperandType : type_t {
    None          = 0,
    Constant,     // Simple constant
    Register,     // Register
    Immediate,    // Immediate Value
    Memory,       // Direct Memory Pointer
    Displacement, // Indirect Memory Pointer
};

enum class OperandFlags: flag_t {
    None          = (1 << 0),
    Local         = (1 << 1),  // Local Variable
    Argument      = (1 << 2),  // Function Argument
    Target        = (1 << 3),  // Branch destination
};

ENUM_FLAGS_OPERATORS(OperandFlags)

struct RegisterOperand
{
    register_id_t r{-1};
    tag_t tag;
};

FORCE_STANDARD_LAYOUT(RegisterOperand);

struct DisplacementOperand
{
    RegisterOperand basestruct, indexstruct;
    s64 scale{1};
    s64 displacement;
};

FORCE_STANDARD_LAYOUT(DisplacementOperand);

struct Operand
{
    OperandType type;
    OperandFlags flags;
    tag_t tag;
    u32 size;
    size_t index{REDasm::npos}, loc_index{REDasm::npos};
    RegisterOperand reg;
    DisplacementOperand disp;
    union { s64 s_value; u64 u_value; };

    void asTarget();
    bool checkCharacter();
    bool isCharacter() const;
    bool isNumeric() const;
    bool isTarget() const;
    bool isIndexValid() const;
    bool isBaseValid() const;
    bool displacementIsDynamic() const;
    bool displacementCanBeAddress() const;
};

FORCE_STANDARD_LAYOUT(Operand);

} // namespace REDasm
