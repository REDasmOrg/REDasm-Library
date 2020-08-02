#include "llfunction.h"
#include "../context.h"

LLFunction::LLFunction(Disassembler* disassembler, rd_address address): m_disassembler(disassembler), m_address(address) { }

const RDILExpression* LLFunction::instruction(size_t idx) const
{
    if(idx >= m_instructions.size()) return nullptr;
    return m_instructions[idx].get();
}

size_t LLFunction::size() const { return m_instructions.size(); }
void LLFunction::append(RDILExpression* e) { if(e) m_instructions.emplace_back(e); }
RDILExpression* LLFunction::exprPOP(rd_size size) const { return this->expr(RDIL_Pop, size); }
RDILExpression* LLFunction::exprPUSH(rd_size size, rd_size value) const { return this->exprVALUE(RDIL_Push, size, value); }
RDILExpression* LLFunction::exprREG(rd_size size, rd_register_id reg) const { return this->exprVALUE(RDIL_Reg, size, reg); }
RDILExpression* LLFunction::exprCNST(rd_size size, rd_size value) const { return this->exprVALUE(RDIL_Cnst, size, value); }
RDILExpression* LLFunction::exprADD(rd_size size, RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Add, size, l, r); }
RDILExpression* LLFunction::exprSUB(rd_size size, RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Sub, size, l, r); }
RDILExpression* LLFunction::exprMUL(rd_size size, RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Mul, size, l, r); }
RDILExpression* LLFunction::exprDIV(rd_size size, RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Div, size, l, r); }
RDILExpression* LLFunction::exprAND(rd_size size, RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Div, size, l, r); }
RDILExpression* LLFunction::exprOR(rd_size size, RDILExpression* l, RDILExpression* r)  const { return this->exprLR(RDIL_Or, size, l, r);  }
RDILExpression* LLFunction::exprXOR(rd_size size, RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Xor, size, l, r); }
RDILExpression* LLFunction::exprCE(rd_size size, RDILExpression* l, RDILExpression* r)  const { return this->exprLR(RDIL_Ce, size, l, r);  }
RDILExpression* LLFunction::exprCNE(rd_size size, RDILExpression* l, RDILExpression* r) const { return this->exprLR(RDIL_Cne, size, l, r); }
RDILExpression* LLFunction::exprLOAD(rd_size size, RDILExpression* dst, RDILExpression* src) const { return this->exprDS(RDIL_Cne, size, dst, src); }
RDILExpression* LLFunction::exprSTORE(rd_size size, RDILExpression* dst, RDILExpression* src) const { return this->exprDS(RDIL_Cne, size, dst, src); }
RDILExpression* LLFunction::exprCOPY(rd_size size, RDILExpression* dst, RDILExpression* src) const { return this->exprDS(RDIL_Cne, size, dst, src); }

RDILExpression* LLFunction::exprIF(rd_size size, RDILExpression* cond, RDILExpression* t, RDILExpression* f) const
{
    auto* expr = static_cast<RDILExpressionCMP*>(this->expr(RDIL_If, size));
    expr->cond.reset(cond);
    expr->t.reset(t);
    expr->f.reset(f);
    return expr;
}

RDILExpression* LLFunction::exprVALUE(rd_type type, rd_size size, rd_size value) const
{
    auto* expr = static_cast<RDILExpressionValue*>(this->expr(type, size));
    expr->value = value;
    return expr;
}

RDILExpression* LLFunction::exprLR(rd_type type, rd_size size, RDILExpression* l, RDILExpression* r) const
{
    auto* expr = static_cast<RDILExpressionLR*>(this->expr(type, size));
    expr->left.reset(l);
    expr->right.reset(r);
    return expr;
}

RDILExpression* LLFunction::exprDS(rd_type type, rd_size size, RDILExpression* dst, RDILExpression* src) const
{
    auto* expr = static_cast<RDILExpressionDS*>(this->expr(type, size));
    expr->dst.reset(dst);
    expr->src.reset(src);
    return expr;
}

RDILExpression* LLFunction::expr(rd_type type, rd_size size) const
{
    RDILExpression* expr = nullptr;

    switch(type)
    {
        case RDIL_Ret:
        case RDIL_Nop:
        case RDIL_Unknown:
        case RDIL_Pop: expr = new RDILExpression(); break;

        case RDIL_Reg:
        case RDIL_Cnst:
        case RDIL_Push: expr = new RDILExpressionValue(); break;

        case RDIL_Add:
        case RDIL_Sub:
        case RDIL_Mul:
        case RDIL_Div:
        case RDIL_And:
        case RDIL_Or:
        case RDIL_Xor:
        //case RDIL_Not:
        case RDIL_Ce:
        case RDIL_Cne: expr = new RDILExpressionLR(); break;

        case RDIL_Load:
        case RDIL_Store:
        case RDIL_Copy: expr = new RDILExpressionDS(); break;

        case RDIL_If: expr = new RDILExpressionCMP(); break;

        default: rd_ctx->log("Unhandled expression type #" + std::to_string(type)); return nullptr;
    }

    expr->type = type;
    expr->size = size;
    return expr;
}
