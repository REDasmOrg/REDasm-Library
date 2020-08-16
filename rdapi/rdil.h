#pragma once

#include "macros.h"
#include "types.h"

struct RDDisassembler;
struct RDGraph;

enum RDILTypes
{
    RDIL_Unknown,                                         // Special
    RDIL_Nop,                                             // Other
    RDIL_Reg, RDIL_Cnst, RDIL_Addr, RDIL_Var,             // Value
    RDIL_Add, RDIL_Sub, RDIL_Mul, RDIL_Div,               // Math
    RDIL_And, RDIL_Or, RDIL_Xor, RDIL_Not,                // Logic
    RDIL_Load, RDIL_Store, RDIL_Copy,                     // Memory
    RDIL_If, RDIL_Goto, RDIL_Call, RDIL_Ret,              // Control Flow
    RDIL_Eq, RDIL_Ne, RDIL_Lt, RDIL_Le, RDIL_Gt, RDIL_Ge, // Compare
    RDIL_Push, RDIL_Pop                                   // Stack
};

/* *** Tested ***
 * --------------
 * Unknown
 * Nop
 * Reg, Cnst
 * Call
 * Jump
 * Load, Copy
 * Push, Pop
 * Add, Sub, Mul, Div, And, Or, Xor
 */

DECLARE_HANDLE(RDILFunction);
DECLARE_HANDLE(RDExpression);

RD_API_EXPORT RDGraph* RDILGraph_Create(const RDDisassembler* disassembler, rd_address address);

RD_API_EXPORT const RDExpression* RDILFunction_GetExpression(const RDILFunction* rdilfunction, size_t idx);
RD_API_EXPORT const RDExpression* RDILFunction_GetFirstExpression(const RDILFunction* rdilfunction);
RD_API_EXPORT const RDExpression* RDILFunction_GetLastExpression(const RDILFunction* rdilfunction);
RD_API_EXPORT void RDILFunction_Insert(RDILFunction* rdilfunction, size_t idx, RDExpression* expression);
RD_API_EXPORT void RDILFunction_Append(RDILFunction* rdilfunction, RDExpression* expression);
RD_API_EXPORT size_t RDILFunction_Size(const RDILFunction* rdilfunction);
RD_API_EXPORT RDExpression* RDILFunction_UNKNOWN(const RDILFunction* rdilfunction);
RD_API_EXPORT RDExpression* RDILFunction_NOP(const RDILFunction* rdilfunction);
RD_API_EXPORT RDExpression* RDILFunction_VAR(const RDILFunction* rdilfunction, size_t size, const char* name);
RD_API_EXPORT RDExpression* RDILFunction_REG(const RDILFunction* rdilfunction, size_t size, const char* reg);
RD_API_EXPORT RDExpression* RDILFunction_CNST(const RDILFunction* rdilfunction, size_t size, u64 value);
RD_API_EXPORT RDExpression* RDILFunction_ADDR(const RDILFunction* rdilfunction, rd_address address);
RD_API_EXPORT RDExpression* RDILFunction_ADD(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r);
RD_API_EXPORT RDExpression* RDILFunction_SUB(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r);
RD_API_EXPORT RDExpression* RDILFunction_MUL(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r);
RD_API_EXPORT RDExpression* RDILFunction_DIV(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r);
RD_API_EXPORT RDExpression* RDILFunction_AND(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r);
RD_API_EXPORT RDExpression* RDILFunction_OR(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r);
RD_API_EXPORT RDExpression* RDILFunction_XOR(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r);
RD_API_EXPORT RDExpression* RDILFunction_LOAD(const RDILFunction* rdilfunction, RDExpression* memloc);
RD_API_EXPORT RDExpression* RDILFunction_STORE(const RDILFunction* rdilfunction, RDExpression* dst, RDExpression* src);
RD_API_EXPORT RDExpression* RDILFunction_COPY(const RDILFunction* rdilfunction, RDExpression* dst, RDExpression* src);
RD_API_EXPORT RDExpression* RDILFunction_GOTO(const RDILFunction* rdilfunction, RDExpression* e);
RD_API_EXPORT RDExpression* RDILFunction_CALL(const RDILFunction* rdilfunction, RDExpression* e);
RD_API_EXPORT RDExpression* RDILFunction_RET(const RDILFunction* rdilfunction, RDExpression* e);
RD_API_EXPORT RDExpression* RDILFunction_IF(const RDILFunction* rdilfunction, RDExpression* cond, RDExpression* t, RDExpression* f);
RD_API_EXPORT RDExpression* RDILFunction_EQ(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r);
RD_API_EXPORT RDExpression* RDILFunction_NE(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r);
RD_API_EXPORT RDExpression* RDILFunction_LT(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r);
RD_API_EXPORT RDExpression* RDILFunction_LE(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r);
RD_API_EXPORT RDExpression* RDILFunction_GT(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r);
RD_API_EXPORT RDExpression* RDILFunction_GE(const RDILFunction* rdilfunction, RDExpression* l, RDExpression* r);
RD_API_EXPORT RDExpression* RDILFunction_PUSH(const RDILFunction* rdilfunction, RDExpression* e);
RD_API_EXPORT RDExpression* RDILFunction_POP(const RDILFunction* rdilfunction, RDExpression* e);
