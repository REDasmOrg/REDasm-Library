#pragma once

#include <rdapi/types.h>
#include <forward_list>
#include <unordered_map>
#include <vector>
#include "expressions.h"
#include "../object.h"
#include <set>

class Disassembler;

class ILFunction: public Object
{
    public:
        ILFunction(Context* ctx);
        void insert(size_t idx, ILExpression* e);
        void append(ILExpression* e);
        void setCurrentAddress(rd_address address);
        bool getAddress(const ILExpression* e, rd_address* address) const;
        const ILExpression* first() const;
        const ILExpression* last() const;
        const ILExpression* expression(size_t idx) const;
        bool empty() const;
        size_t size() const;

    public:
        static bool generate(rd_address address, ILFunction* il);

    public:
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
        ILExpression* exprADD(ILExpression* l, ILExpression* r) const;
        ILExpression* exprSUB(ILExpression* l, ILExpression* r) const;
        ILExpression* exprMUL(ILExpression* l, ILExpression* r) const;
        ILExpression* exprDIV(ILExpression* l, ILExpression* r) const;
        ILExpression* exprAND(ILExpression* l, ILExpression* r) const;
        ILExpression* exprOR(ILExpression* l, ILExpression* r)  const;
        ILExpression* exprXOR(ILExpression* l, ILExpression* r) const;
        ILExpression* exprCOPY(ILExpression* dst, ILExpression* src) const;
        ILExpression* exprIF(ILExpression* cond, ILExpression* t, ILExpression* f) const;
        ILExpression* exprEQ(ILExpression* l, ILExpression* r)  const;
        ILExpression* exprNE(ILExpression* l, ILExpression* r) const;
        ILExpression* exprLT(ILExpression* l, ILExpression* r) const;
        ILExpression* exprLE(ILExpression* l, ILExpression* r) const;
        ILExpression* exprGT(ILExpression* l, ILExpression* r) const;
        ILExpression* exprGE(ILExpression* l, ILExpression* r) const;

    private:
        ILExpression* check(ILExpression* e) const;
        ILExpression* exprVALUE_P(rd_type rdil, size_t size, uintptr_t value) const;
        ILExpression* exprVALUE(rd_type rdil, size_t size, u64 value) const;
        ILExpression* exprLR(rd_type rdil, size_t size, ILExpression* l, ILExpression* r) const;
        ILExpression* exprDS(rd_type rdil, size_t size, ILExpression* dst, ILExpression* src) const;
        ILExpression* exprU(rd_type rdil, size_t size, ILExpression* e) const;
        ILExpression* expr(rd_type rdil, size_t size) const;
        ILExpression* expr(rd_type rdil) const;

    private:
        static bool generatePath(rd_address address, ILFunction* il, std::set<rd_address>& path);
        static void generateBasicBlock(rd_address address, ILFunction* il, std::set<rd_address>& path);

    private:
        rd_address m_currentaddress{RD_NPOS};
        std::vector<ILExpression*> m_expressions;
        std::unordered_map<const ILExpression*, rd_address> m_addresses;
        mutable std::forward_list<ILExpressionPtr> m_pool;
        const Disassembler* m_disassembler;
};
