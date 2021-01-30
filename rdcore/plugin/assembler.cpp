#include "assembler.h"
#include "../support/utils.h"
#include "../rdil/ilfunction.h"
#include <climits>

Assembler::Assembler(const RDEntryAssembler* entry, Context* ctx): Entry<RDEntryAssembler>(entry, ctx) { }

void Assembler::lift(rd_address address, const RDBufferView* view, ILFunction* il) const
{
    il->setCurrentAddress(address); // Keep address <-> RDIL reference
    if(m_entry->lift) m_entry->lift(CPTR(RDContext, this->context()), address, view, CPTR(RDILFunction, il));
    if(!m_entry->lift || il->empty()) il->append(il->exprUNKNOWN());
}

bool Assembler::encode(RDEncodedInstruction* encoded) const
{
    if(!encoded->decoded || !m_entry->encode) return false;
    return m_entry->encode(m_entry, encoded);
}

void Assembler::emulate(EmulateResult* result) const { if(m_entry->emulate) m_entry->emulate(CPTR(RDContext, this->context()), CPTR(RDEmulateResult, result)); }

bool Assembler::renderInstruction(const RDRendererParams* rp)
{
    if(!m_entry->renderinstruction) return false;
    m_entry->renderinstruction(CPTR(RDContext, this->context()), rp);
    return true;
}

size_t Assembler::addressWidth() const { return m_entry->bits / CHAR_BIT; }
size_t Assembler::bits() const { return m_entry->bits; }
