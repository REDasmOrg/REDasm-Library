#pragma once

#include <rdapi/rdil.h>
#include <unordered_set>
#include <memory>
#include <list>
#include "../object.h"

class ILExpression;
typedef std::unique_ptr<ILExpression> ILExpressionPtr;

class ILExpression : public Object
{
    public:
        rd_type type{RDIL_Unknown};
        size_t size{0};

        union { ILExpression *n1{nullptr}, *u, *cond; };
        union { ILExpression *n2{nullptr}, *dst, *left, *t; };
        union { ILExpression *n3{nullptr}, *src, *right, *f; };
        union { PRIVATE_RDIL_VALUE_FIELDS };

    public:
        static ILExpression* clone(const ILExpression* e);

    private:
        std::unordered_set<ILExpressionPtr> children; // Keep expressions alive, if needed

    friend class ILFunction;
};

class ILExpressionTree: public Object
{
    public:
        ILExpressionTree();
        ILExpressionTree(Context* ctx);
        ILExpression* exprUNKNOWN() const;
        ILExpression* exprNOP() const;
        ILExpression* exprPOP(ILExpression* e) const;
        ILExpression* exprPUSH(ILExpression* e) const;
        ILExpression* exprREG(size_t size, const char* reg) const;
        ILExpression* exprCNST(size_t size, u64 value) const;
        ILExpression* exprVAR(size_t size, const char* name) const;
        ILExpression* exprJUMP(ILExpression* e) const;
        ILExpression* exprCALL(ILExpression* e) const;
        ILExpression* exprRET(ILExpression* e) const;
        ILExpression* exprMEM(ILExpression* e) const;
        ILExpression* exprNOT(ILExpression* e) const;
        ILExpression* exprADD(ILExpression* l, ILExpression* r) const;
        ILExpression* exprSUB(ILExpression* l, ILExpression* r) const;
        ILExpression* exprMUL(ILExpression* l, ILExpression* r) const;
        ILExpression* exprDIV(ILExpression* l, ILExpression* r) const;
        ILExpression* exprMOD(ILExpression* l, ILExpression* r) const;
        ILExpression* exprAND(ILExpression* l, ILExpression* r) const;
        ILExpression* exprOR(ILExpression* l, ILExpression* r)  const;
        ILExpression* exprXOR(ILExpression* l, ILExpression* r) const;
        ILExpression* exprLSL(ILExpression* l, ILExpression* r) const;
        ILExpression* exprLSR(ILExpression* l, ILExpression* r) const;
        ILExpression* exprASL(ILExpression* l, ILExpression* r) const;
        ILExpression* exprASR(ILExpression* l, ILExpression* r) const;
        ILExpression* exprROL(ILExpression* l, ILExpression* r) const;
        ILExpression* exprROR(ILExpression* l, ILExpression* r) const;
        ILExpression* exprCOPY(ILExpression* dst, ILExpression* src) const;
        ILExpression* exprIF(ILExpression* cond, ILExpression* t, ILExpression* f) const;
        ILExpression* exprEQ(ILExpression* l, ILExpression* r)  const;
        ILExpression* exprNE(ILExpression* l, ILExpression* r) const;
        ILExpression* exprLT(ILExpression* l, ILExpression* r) const;
        ILExpression* exprLE(ILExpression* l, ILExpression* r) const;
        ILExpression* exprGT(ILExpression* l, ILExpression* r) const;
        ILExpression* exprGE(ILExpression* l, ILExpression* r) const;

    protected:
        ILExpression* check(ILExpression* e) const;

    private:
        ILExpression* exprVALUE_P(rd_type rdil, size_t size, uintptr_t value) const;
        ILExpression* exprVALUE(rd_type rdil, size_t size, u64 value) const;
        ILExpression* exprLR(rd_type rdil, size_t size, ILExpression* l, ILExpression* r) const;
        ILExpression* exprDS(rd_type rdil, size_t size, ILExpression* dst, ILExpression* src) const;
        ILExpression* exprU(rd_type rdil, size_t size, ILExpression* e) const;
        ILExpression* expr(rd_type rdil, size_t size) const;
        ILExpression* expr(rd_type rdil) const;

    private:
        mutable std::list<ILExpressionPtr> m_pool;
};
