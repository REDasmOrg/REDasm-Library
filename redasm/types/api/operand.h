#pragma once

#include "api_base.h"

namespace REDasm {

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
    enum Type: type_t {
        T_None,
        T_Constant,     // Simple constant
        T_Register,     // Register
        T_Immediate,    // Immediate Value
        T_Memory,       // Direct Memory Pointer
        T_Displacement, // Indirect Memory Pointer
    };

    enum Flags: flag_t {
        F_None,
        F_Local         = (1 << 1),  // Local Variable
        F_Argument      = (1 << 2),  // Function Argument
        F_Target        = (1 << 3),  // Branch destination
    };

    type_t type;
    flag_t flags;
    tag_t tag;
    u32 size;
    size_t index{REDasm::npos}, loc_index{REDasm::npos};
    RegisterOperand reg;
    DisplacementOperand disp;
    union { s64 s_value; u64 u_value; };

    void asTarget();

    bool isConstant() const;
    bool isRegister() const;
    bool isImmediate() const;
    bool isMemory() const;
    bool isDisplacement() const;

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
