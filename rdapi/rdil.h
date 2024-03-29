#pragma once

#include "macros.h"
#include "types.h"

/*
 * *** RDIL Opcode Table ***
   +---------+-----------+-----------+-----------+----------------------------------+
   | Opcode  | Operand 1 | Operand 2 | Operand 3 |           Description            |
   +---------+-----------+-----------+-----------+----------------------------------+
   | Reg     |    N/A    |    N/A    |    N/A    | (VALUE) Register Name            |
   | Cnst    |    N/A    |    N/A    |    N/A    | (VALUE) Constant Value           |
   | Var     |    N/A    |    N/A    |    N/A    | (VALUE) Symbol Name              |
   | Unknown |           |           |           |                                  |
   | Nop     |           |           |           | nop                              |
   | Add     | left      | right     |           | left + right                     |
   | Sub     | left      | right     |           | left - right                     |
   | Mul     | left      | right     |           | left * right                     |
   | Div     | left      | right     |           | left / right                     |
   | Mod     | left      | right     |           | left % right                     |
   | And     | left      | right     |           | left & right                     |
   | Or      | left      | right     |           | left | right                     |
   | Xor     | left      | right     |           | left ^ right                     |
   | Lsl     | left      | right     |           | left << right                    |
   | Lsr     | left      | right     |           | left >> right                    |
   | Asl     | left      | right     |           | left <<< right                   |
   | Asr     | left      | right     |           | left >>> right                   |
   | Eq      | left      | right     |           | left == right                    |
   | Ne      | left      | right     |           | left != right                    |
   | Lt      | left      | right     |           | left < right                     |
   | Le      | left      | right     |           | left >= right                    |
   | Gt      | left      | right     |           | left > right                     |
   | Ge      | left      | right     |           | left >= right                    |
   | Rol     | left      | right     |           | rotateleft(left, right)          |
   | Ror     | left      | right     |           | rotateright(left, right)         |
   | Not     | u         |           |           | ~u                               |
   | Mem     | u         |           |           | [u]                              |
   | Goto    | u         |           |           | goto(u)                          |
   | Call    | u         |           |           | call(u)                          |
   | Push    | u         |           |           | push(u)                          |
   | Pop     | u         |           |           | u = pop()                        |
   | Copy    | dst       | src       |           | dst = src                        |
   | If      | cond      | t         | f         | if (cond) t else f               |
   | Ret     | cond      |           |           | ret(cond)                        |
   | Int     | u         |           |           | int u                            |
   +---------+-----------+-----------+-----------+----------------------------------+
 */

struct RDContext;
struct RDGraph;

enum RDILTypes
{
    RDIL_Unknown,                                         // Special
    RDIL_Nop,                                             // Other
    RDIL_Reg, RDIL_Cnst, RDIL_Var,                        // Value
    RDIL_Add, RDIL_Sub, RDIL_Mul, RDIL_Div, RDIL_Mod,     // Math
    RDIL_And, RDIL_Or, RDIL_Xor, RDIL_Not,                // Logic
    RDIL_Lsl, RDIL_Lsr, RDIL_Asl, RDIL_Asr,               // Shift
    RDIL_Rol, RDIL_Ror,                                   // Rotate
    RDIL_Mem, RDIL_Copy,                                  // R/W
    RDIL_If, RDIL_Goto, RDIL_Call, RDIL_Ret,              // Control Flow
    RDIL_Eq, RDIL_Ne, RDIL_Lt, RDIL_Le, RDIL_Gt, RDIL_Ge, // Compare
    RDIL_Push, RDIL_Pop,                                  // Stack
    RDIL_Int,                                             // Privileged
};

#define PRIVATE_RDIL_VALUE_FIELDS \
    uintptr_t value; \
    rd_address address; \
    rd_offset offset; \
    rd_location location; \
    u64 u_value; \
    s64 s_value; \
    const char* reg; \
    const char* var;

typedef struct RDILValue
{
    rd_type type;
    union { PRIVATE_RDIL_VALUE_FIELDS };
} RDILValue;

RD_HANDLE(RDILFunction);
RD_HANDLE(RDILExpression);

RD_API_EXPORT RDGraph* RDILGraph_Create(RDContext* ctx, rd_address address);

RD_API_EXPORT RDILExpression* RDILExpression_Create(RDContext* ctx, rd_address address);
RD_API_EXPORT rd_type RDILExpression_Type(const RDILExpression* e);
RD_API_EXPORT bool RDILExpression_Match(const RDILExpression* e, const char* m);
RD_API_EXPORT bool RDILExpression_GetValue(const RDILExpression* e, RDILValue* value);
RD_API_EXPORT const char* RDILExpression_GetText(const RDILExpression* e);
RD_API_EXPORT const char* RDILExpression_GetFormat(const RDILExpression* e);
RD_API_EXPORT size_t RDILExpression_Extract(const RDILExpression* e, const RDILValue** values);

RD_API_EXPORT RDILFunction* RDILFunction_Create(RDContext* context, rd_address address);
RD_API_EXPORT const RDILExpression* RDILFunction_GetExpression(const RDILFunction* rdilfunction, size_t idx);
RD_API_EXPORT const RDILExpression* RDILFunction_GetFirstExpression(const RDILFunction* rdilfunction);
RD_API_EXPORT const RDILExpression* RDILFunction_GetLastExpression(const RDILFunction* rdilfunction);
RD_API_EXPORT size_t RDILFunction_Extract(const RDILFunction* rdilfunction, const RDILValue** values);
RD_API_EXPORT bool RDILFunction_Match(const RDILFunction* rdilfunction, const char* m);
RD_API_EXPORT bool RDILFunction_GetAddress(const RDILFunction* rdilfunction, const RDILExpression* e, rd_address* address);
RD_API_EXPORT void RDILFunction_Insert(RDILFunction* rdilfunction, size_t idx, RDILExpression* expression);
RD_API_EXPORT void RDILFunction_Append(RDILFunction* rdilfunction, RDILExpression* expression);
RD_API_EXPORT size_t RDILFunction_Size(const RDILFunction* rdilfunction);
RD_API_EXPORT RDILExpression* RDILFunction_UNKNOWN(const RDILFunction* rdilfunction);
RD_API_EXPORT RDILExpression* RDILFunction_NOP(const RDILFunction* rdilfunction);
RD_API_EXPORT RDILExpression* RDILFunction_VAR(const RDILFunction* rdilfunction, size_t size, const char* name);
RD_API_EXPORT RDILExpression* RDILFunction_REG(const RDILFunction* rdilfunction, size_t size, const char* reg);
RD_API_EXPORT RDILExpression* RDILFunction_CNST(const RDILFunction* rdilfunction, size_t size, u64 value);
RD_API_EXPORT RDILExpression* RDILFunction_ADD(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_SUB(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_MUL(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_DIV(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_MOD(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_AND(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_OR(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_XOR(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_LSL(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_LSR(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_ASL(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_ASR(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_ROL(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_ROR(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_NOT(const RDILFunction* rdilfunction, RDILExpression* e);
RD_API_EXPORT RDILExpression* RDILFunction_MEM(const RDILFunction* rdilfunction, RDILExpression* e);
RD_API_EXPORT RDILExpression* RDILFunction_COPY(const RDILFunction* rdilfunction, RDILExpression* dst, RDILExpression* src);
RD_API_EXPORT RDILExpression* RDILFunction_GOTO(const RDILFunction* rdilfunction, RDILExpression* e);
RD_API_EXPORT RDILExpression* RDILFunction_CALL(const RDILFunction* rdilfunction, RDILExpression* e);
RD_API_EXPORT RDILExpression* RDILFunction_RET(const RDILFunction* rdilfunction, RDILExpression* e);
RD_API_EXPORT RDILExpression* RDILFunction_IF(const RDILFunction* rdilfunction, RDILExpression* cond, RDILExpression* t, RDILExpression* f);
RD_API_EXPORT RDILExpression* RDILFunction_EQ(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_NE(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_LT(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_LE(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_GT(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_GE(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_PUSH(const RDILFunction* rdilfunction, RDILExpression* e);
RD_API_EXPORT RDILExpression* RDILFunction_POP(const RDILFunction* rdilfunction, RDILExpression* e);
RD_API_EXPORT RDILExpression* RDILFunction_INT(const RDILFunction* rdilfunction, RDILExpression* e);
