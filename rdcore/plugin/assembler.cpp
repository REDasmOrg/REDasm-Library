#include "assembler.h"
#include "../support/utils.h"
#include "../rdil/ilcpu.h"
#include <climits>

Assembler::Assembler(RDAssemblerPlugin* passembler, Disassembler* disassembler): m_passembler(passembler), m_disassembler(disassembler) { }

RDInstruction* Assembler::emitRDIL(const RDInstruction* instruction, size_t* len)
{
    ILCPU::init(m_rdilres.data());
    RDInstruction* rdilend = m_rdilres.data();

    if(m_passembler->rdil)
        m_passembler->rdil(m_passembler, instruction, &rdilend);

    if(len) *len = std::max<size_t>(1, (rdilend - m_rdilres.data()) + 1);
    return m_rdilres.data();
}

std::string Assembler::registerName(const RDInstruction* instruction, const RDOperand* op, rd_register_id r) const
{
    if(m_passembler->regname)
    {
        const char* rn = m_passembler->regname(m_passembler, instruction, op, r);
        if(rn) return rn;
    }

    return "$" + Utils::number(r);
}

bool Assembler::isStop(const RDInstruction* instruction) const
{
    if(m_passembler->isstop) return m_passembler->isstop(m_passembler, CPTR(const RDDisassembler, m_disassembler), instruction);

    if(HAS_FLAG(instruction, InstructionFlags_Stop)) return true;
    if(IS_TYPE(instruction, InstructionType_Jump) && !HAS_FLAG(instruction, InstructionFlags_Conditional)) return true;
    return false;
}

bool Assembler::decode(BufferView* view, RDInstruction* instruction) const
{
    if(!m_passembler->decode) return false;
    return m_passembler->decode(m_passembler, CPTR(RDBufferView, view), instruction);
}

bool Assembler::encode(RDEncodedInstruction* encoded) const
{
    if(!encoded->decoded || !m_passembler->encode) return false;
    return m_passembler->encode(m_passembler, encoded);
}

bool Assembler::render(RDRenderItemParams* rip) const
{
    if(!m_passembler->render) return false;
    return m_passembler->render(m_passembler, rip);
}

void Assembler::emulate(const RDInstruction* instruction)
{
    if(!m_passembler->emulate) return;
    m_passembler->emulate(m_passembler, CPTR(RDDisassembler, m_disassembler), instruction);
}

void Assembler::rdil(const RDInstruction* instruction)
{
    if(!m_passembler->rdil) return;

    //RDInstruction rdil;
    //if(!m_passembler->rdil(m_passembler, instruction, &rdil)) return;
}

const char* Assembler::id() const { return m_passembler->id; }
size_t Assembler::addressWidth() const { return m_passembler->bits / CHAR_BIT; }
size_t Assembler::bits() const { return m_passembler->bits; }
