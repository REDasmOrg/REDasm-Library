#include "assembler.h"
#include "../support/utils.h"
#include "../rdil/ilfunction.h"
#include <climits>

Assembler::Assembler(RDAssemblerPlugin* passembler, Disassembler* disassembler): m_passembler(passembler), m_disassembler(disassembler) { }

void Assembler::lift(rd_address address, const RDBufferView* view, ILFunction* il) const
{
    il->setCurrentAddress(address); // Keep address <-> RDIL reference
    if(m_passembler->lift) m_passembler->lift(m_passembler, address, view, CPTR(RDILFunction, il));
    else il->append(il->exprUNKNOWN());
}

bool Assembler::encode(RDEncodedInstruction* encoded) const
{
    if(!encoded->decoded || !m_passembler->encode) return false;
    return m_passembler->encode(m_passembler, encoded);
}

void Assembler::emulate(EmulateResult* result) const
{
    if(!m_passembler->emulate) return;
    m_passembler->emulate(m_passembler, CPTR(RDEmulateResult, result));
}

bool Assembler::getUserData(RDUserData* userdata) const
{
    if(!userdata) return false;
    userdata->userdata = m_passembler->userdata;
    return true;
}

bool Assembler::renderInstruction(RDRenderItemParams* rip)
{
    if(!m_passembler->renderinstruction) return false;
    m_passembler->renderinstruction(m_passembler, rip);
    return true;
}

const char* Assembler::id() const { return m_passembler->id; }
size_t Assembler::addressWidth() const { return m_passembler->bits / CHAR_BIT; }
size_t Assembler::bits() const { return m_passembler->bits; }
