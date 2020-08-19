#pragma once

#include <rdapi/types.h>
#include <forward_list>
#include <vector>
#include "expressions.h"

class Disassembler;

class ILFunction
{
    public:
        ILFunction(const Disassembler* disassembler);
        void insert(size_t idx, RDILExpression* e);
        void append(RDILExpression* e);
        const RDILExpression* first() const;
        const RDILExpression* last() const;
        const RDILExpression* expression(size_t idx) const;
        bool empty() const;
        size_t size() const;

    public:
        static bool generate(const Disassembler* disassembler, rd_address address, ILFunction* il);

    public:
        RDILExpression* exprUNKNOWN() const;
        RDILExpression* exprNOP() const;
        RDILExpression* exprPOP(RDILExpression* e) const;
        RDILExpression* exprPUSH(RDILExpression* e) const;
        RDILExpression* exprREG(size_t size, const char* reg) const;
        RDILExpression* exprCNST(size_t size, u64 value) const;
        RDILExpression* exprVAR(size_t size, const char* name) const;
        RDILExpression* exprADDR(rd_address addr) const;
        RDILExpression* exprJUMP(RDILExpression* e) const;
        RDILExpression* exprCALL(RDILExpression* e) const;
        RDILExpression* exprRET(RDILExpression* e) const;
        RDILExpression* exprLOAD(RDILExpression* memloc) const;
        RDILExpression* exprSTORE( RDILExpression* dst, RDILExpression* src) const;
        RDILExpression* exprADD(RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprSUB(RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprMUL(RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprDIV(RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprAND(RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprOR(RDILExpression* l, RDILExpression* r)  const;
        RDILExpression* exprXOR(RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprCOPY(RDILExpression* dst, RDILExpression* src) const;
        RDILExpression* exprIF(RDILExpression* cond, RDILExpression* t, RDILExpression* f) const;
        RDILExpression* exprEQ(RDILExpression* l, RDILExpression* r)  const;
        RDILExpression* exprNE(RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprLT(RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprLE(RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprGT(RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprGE(RDILExpression* l, RDILExpression* r) const;

    private:
        RDILExpression* check(RDILExpression* e) const;
        RDILExpression* exprVALUE_P(rd_type rdil, size_t size, uintptr_t value) const;
        RDILExpression* exprVALUE(rd_type rdil, size_t size, u64 value) const;
        RDILExpression* exprLR(rd_type rdil, size_t size, RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprDS(rd_type rdil, size_t size, RDILExpression* dst, RDILExpression* src) const;
        RDILExpression* exprU(rd_type rdil, size_t size, RDILExpression* e) const;
        RDILExpression* expr(rd_type rdil, size_t size) const;
        RDILExpression* expr(rd_type rdil) const;

    private:
        std::vector<RDILExpression*> m_expressions;
        mutable std::forward_list<RDILExpressionPtr> m_pool;
        const Disassembler* m_disassembler;
};
