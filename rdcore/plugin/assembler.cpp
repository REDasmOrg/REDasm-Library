#include "assembler.h"
#include <climits>

Assembler::Assembler(const RDAssemblerPlugin* descriptor): m_plugin(descriptor) { }
size_t Assembler::addressWidth() const { return this->bits() / CHAR_BIT; }
size_t Assembler::bits() const { return m_plugin->bits; }

bool Assembler::decode(BufferView* bufferview, RDInstruction* instruction)
{
    if(!m_plugin->decode) return false;
    return m_plugin->decode(m_plugin, reinterpret_cast<RDBufferView*>(bufferview), instruction);
}
