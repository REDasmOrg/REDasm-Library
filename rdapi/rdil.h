#pragma once

#include "macros.h"
#include "types.h"

struct RDContext;
struct RDGraph;

enum RDILTypes
{
    RDIL_Unknown,                                         // Special
    RDIL_Nop,                                             // Other
    RDIL_Reg, RDIL_Cnst, RDIL_Var,                        // Value
    RDIL_Add, RDIL_Sub, RDIL_Mul, RDIL_Div,               // Math
    RDIL_And, RDIL_Or, RDIL_Xor, RDIL_Not,                // Logic
    RDIL_Load, RDIL_Store, RDIL_Copy,                     // Memory
    RDIL_If, RDIL_Goto, RDIL_Call, RDIL_Ret,              // Control Flow
    RDIL_Eq, RDIL_Ne, RDIL_Lt, RDIL_Le, RDIL_Gt, RDIL_Ge, // Compare
    RDIL_Push, RDIL_Pop                                   // Stack
};

#define PRIVATE_RDIL_VALUE_FIELDS \
    uintptr_t value; \
    rd_address address; \
    rd_offset offset; \
    rd_location location; \
    u64 u_value; \
    s64 s_value; \
    const char* reg; \
    const char* var; \

typedef union RDILValue { PRIVATE_RDIL_VALUE_FIELDS } RDILValue;

DECLARE_HANDLE(RDILFunction);
DECLARE_HANDLE(RDILExpression);

RD_API_EXPORT RDGraph* RDILGraph_Create(RDContext* ctx, rd_address address);

RD_API_EXPORT rd_type RDILExpression_Type(const RDILExpression* e);
RD_API_EXPORT size_t RDILExpression_Size(const RDILExpression* e);
RD_API_EXPORT bool RDILExpression_Match(const RDILExpression* e, const char* m);
RD_API_EXPORT bool RDILExpression_GetValue(const RDILExpression* e, RDILValue* value);
RD_API_EXPORT const RDILExpression* RDILExpression_GetN1(const RDILExpression* e);
RD_API_EXPORT const RDILExpression* RDILExpression_GetN2(const RDILExpression* e);
RD_API_EXPORT const RDILExpression* RDILExpression_GetN3(const RDILExpression* e);
RD_API_EXPORT const RDILExpression* RDILExpression_GetU(const RDILExpression* e);
RD_API_EXPORT const RDILExpression* RDILExpression_GetCond(const RDILExpression* e);
RD_API_EXPORT const RDILExpression* RDILExpression_GetDst(const RDILExpression* e);
RD_API_EXPORT const RDILExpression* RDILExpression_GetLeft(const RDILExpression* e);
RD_API_EXPORT const RDILExpression* RDILExpression_GetT(const RDILExpression* e);
RD_API_EXPORT const RDILExpression* RDILExpression_GetSrc(const RDILExpression* e);
RD_API_EXPORT const RDILExpression* RDILExpression_GetRight(const RDILExpression* e);
RD_API_EXPORT const RDILExpression* RDILExpression_GetF(const RDILExpression* e);

RD_API_EXPORT RDILFunction* RDILFunction_Generate(RDContext* context, rd_address address);
RD_API_EXPORT const RDILExpression* RDILFunction_GetExpression(const RDILFunction* rdilfunction, size_t idx);
RD_API_EXPORT const RDILExpression* RDILFunction_GetFirstExpression(const RDILFunction* rdilfunction);
RD_API_EXPORT const RDILExpression* RDILFunction_GetLastExpression(const RDILFunction* rdilfunction);
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
RD_API_EXPORT RDILExpression* RDILFunction_AND(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_OR(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_XOR(const RDILFunction* rdilfunction, RDILExpression* l, RDILExpression* r);
RD_API_EXPORT RDILExpression* RDILFunction_LOAD(const RDILFunction* rdilfunction, RDILExpression* memloc);
RD_API_EXPORT RDILExpression* RDILFunction_STORE(const RDILFunction* rdilfunction, RDILExpression* dst, RDILExpression* src);
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
