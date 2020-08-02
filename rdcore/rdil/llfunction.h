#pragma once

#include <rdapi/types.h>
#include <vector>
#include "expressions.h"

class Disassembler;

class LLFunction
{
    public:
        LLFunction(Disassembler* disassembler, rd_address address);
        void append(RDILExpression* e);
        const RDILExpression* instruction(size_t idx) const;
        size_t size() const;

    public:
        RDILExpression* exprPOP(rd_size size) const;
        RDILExpression* exprPUSH(rd_size size, rd_size value) const;
        RDILExpression* exprREG(rd_size size, rd_register_id reg) const;
        RDILExpression* exprCNST(rd_size size, rd_size value) const;
        RDILExpression* exprADD(rd_size size, RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprSUB(rd_size size, RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprMUL(rd_size size, RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprDIV(rd_size size, RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprAND(rd_size size, RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprOR(rd_size size, RDILExpression* l, RDILExpression* r)  const;
        RDILExpression* exprXOR(rd_size size, RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprCE(rd_size size, RDILExpression* l, RDILExpression* r)  const;
        RDILExpression* exprCNE(rd_size size, RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprLOAD(rd_size size, RDILExpression* dst, RDILExpression* src) const;
        RDILExpression* exprSTORE(rd_size size, RDILExpression* dst, RDILExpression* src) const;
        RDILExpression* exprCOPY(rd_size size, RDILExpression* dst, RDILExpression* src) const;
        RDILExpression* exprIF(rd_size size, RDILExpression* cond, RDILExpression* t, RDILExpression* f) const;

    private:
        RDILExpression* exprVALUE(rd_type type, rd_size size, rd_size value) const;
        RDILExpression* exprLR(rd_type type, rd_size size, RDILExpression* l, RDILExpression* r) const;
        RDILExpression* exprDS(rd_type type, rd_size size, RDILExpression* dst, RDILExpression* src) const;
        RDILExpression* expr(rd_type type, rd_size size) const;

    private:
        std::vector<RDILExpressionPtr> m_instructions;
        Disassembler* m_disassembler;
        rd_address m_address;
};
