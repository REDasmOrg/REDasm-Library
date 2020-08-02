#pragma once

#include <rdapi/types.h>
#include <memory>

enum RDILTypes
{
    RDIL_Unknown,                           // Special
    RDIL_Nop,                               // Other
    RDIL_Reg, RDIL_Cnst,                    // Value
    RDIL_Add, RDIL_Sub, RDIL_Mul, RDIL_Div, // Math
    RDIL_And, RDIL_Or, RDIL_Xor, RDIL_Not,  // Logic
    RDIL_Load, RDIL_Store, RDIL_Copy,       // Memory
    RDIL_Jump, RDIL_Call, RDIL_Ret,         // Control Flow
    RDIL_If, RDIL_Ce, RDIL_Cne,             // Compare
    RDIL_Push, RDIL_Pop                     // Stack
};

struct RDILExpression { rd_type type{RDIL_Unknown}; rd_size size{0}; };

struct RDILExpressionValue: public RDILExpression
{
    union {
        rd_register_id reg;
        rd_address address;
        rd_offset offset;
        rd_size value;
    };
};

typedef std::unique_ptr<RDILExpression> RDILExpressionPtr;

struct RDILExpressionDS: public RDILExpression { RDILExpressionPtr dst, src; };
struct RDILExpressionLR: public RDILExpression { RDILExpressionPtr left, right; };
struct RDILExpressionCMP: public RDILExpression { RDILExpressionPtr cond, t, f; };
