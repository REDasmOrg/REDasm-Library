#include "ilcpu.h"
#include "../context.h"
#include "../support/utils.h"

void ILCPU::reset()
{
    std::fill_n(m_regs.begin(), m_regs.size(), 0);
    m_gpr.clear();
    m_failed = false;
}

std::optional<u64> ILCPU::readop(const RDOperand& op) const
{
    if(IS_TYPE(&op, OperandType_Register))
    {
        if(HAS_FLAG(&op, OperandFlags_Virtual) && (op.reg < RDILRegister_Count))
            return m_regs[op.reg];

        auto it = m_gpr.find(op.reg);
        if(it != m_gpr.end()) return it->second;
    }
    else if(IS_TYPE(&op, OperandType_Constant) || IS_TYPE(&op, OperandType_Immediate))
        return op.u_value;

    return std::nullopt;
}

void ILCPU::writeop(const RDOperand& op, u64 val)
{
    if(IS_TYPE(&op, OperandType_Register))
    {
        if(HAS_FLAG(&op, OperandFlags_Virtual) && (op.reg < RDILRegister_Count))
            m_regs[op.reg] = val;
        else
            m_gpr[op.reg] = val;
    }
}

void ILCPU::exec(const RDOperand& dst, const RDOperand& op1, const UnaryOpFunc& opfunc)
{
    auto src1 = this->readop(op1);
    if(src1) this->writeop(dst, opfunc(*src1));
}

void ILCPU::exec(const RDOperand& dst, const RDOperand& op1, const RDOperand& op2, const ILCPU::BinaryOpFunc& opfunc)
{
    auto src1 = this->readop(op1);
    if(!src1) return;

    auto src2 = this->readop(op2);
    if(!src2) return;

    this->writeop(dst, opfunc(*src1, *src2));
}

bool ILCPU::expect(const RDInstruction* rdil, const RDOperand& op, rd_type type)
{
    if(IS_TYPE(&op, type)) return true;
    this->fail("Invalid Operand Type @ " + Utils::hex(rdil->address));
    return false;
}

void ILCPU::init(RDInstruction* instruction)
{
    *instruction = { };
    instruction->id = RDIL_Unknown;
}

bool ILCPU::reg(rd_register_id r, u64* value) const
{
    auto it = m_gpr.find(r);
    if(it == m_gpr.end()) return false;
    if(value) *value = it->second;
    return true;
}

void ILCPU::exec(const RDInstruction* rdil)
{
    const RDOperand& opdst = rdil->operands[0];
    const RDOperand& opsrc1 = rdil->operands[1];
    const RDOperand& opsrc2 = rdil->operands[2];

    switch(rdil->id)
    {
        case RDIL_Add:
            if(!this->expect(rdil, opdst, OperandType_Register)) return;
            this->exec(opdst, opsrc1, opsrc2, [](u64 src1, u64 src2) { return src1 + src2; });
            break;

        case RDIL_Sub:
            if(!this->expect(rdil, opdst, OperandType_Register)) return;
            this->exec(opdst, opsrc1, opsrc2, [](u64 src1, u64 src2) { return src1 - src2; });
            break;

        case RDIL_Mul:
            if(!this->expect(rdil, opdst, OperandType_Register)) return;
            this->exec(opdst, opsrc1, opsrc2, [](u64 src1, u64 src2) { return src1 * src2; });
            break;

        case RDIL_Div:
            if(!this->expect(rdil, opdst, OperandType_Register)) return;
            this->exec(opdst, opsrc1, opsrc2, [](u64 src1, u64 src2) { return src1 / src2; });
            break;

        case RDIL_Mod:
            if(!this->expect(rdil, opdst, OperandType_Register)) return;
            this->exec(opdst, opsrc1, opsrc2, [](u64 src1, u64 src2) { return src1 % src2; });
            break;

        case RDIL_And:
            if(!this->expect(rdil, opdst, OperandType_Register)) return;
            this->exec(opdst, opsrc1, opsrc2, [](u64 src1, u64 src2) { return src1 & src2; });
            break;

        case RDIL_Or:
            if(!this->expect(rdil, opdst, OperandType_Register)) return;
            this->exec(opdst, opsrc1, opsrc2, [](u64 src1, u64 src2) { return src1 | src2; });
            break;

        case RDIL_Xor:
            if(!this->expect(rdil, opdst, OperandType_Register)) return;
            this->exec(opdst, opsrc1, opsrc2, [](u64 src1, u64 src2) { return src1 ^ src2; });
            break;

        case RDIL_Not:
            if(!this->expect(rdil, opdst, OperandType_Register)) return;
            this->exec(opdst, opsrc1, [](u64 src1) { return ~src1; });
            break;

        case RDIL_Undef:
            if(!this->expect(rdil, opdst, OperandType_Register)) return;
            m_gpr.erase(opdst.reg);
            break;

        case RDIL_Unknown:
        case RDIL_Nop: break;

        default: this->fail("Invalid Operation #" + std::to_string(rdil->id)); break;
    }
}

void ILCPU::fail(const std::string& msg)
{
    m_failed = true;
    rd_ctx->log(msg);
}
