#include "assembler_impl.h"
#include <redasm/plugins/assembler/algorithm/algorithm.h>

namespace REDasm {

AssemblerImpl::AssemblerImpl(): PluginImpl(), m_algorithm(nullptr) { }

AssemblerImpl::~AssemblerImpl()
{
    if(m_algorithm)
        delete m_algorithm;

    m_algorithm = nullptr;
}

void AssemblerImpl::setInstructionType(const InstructionPtr &instruction) const
{
    auto it = m_instructiontypes.find(instruction->id);

    if(it != m_instructiontypes.end())
        instruction->type |= it->second;
}

} // namespace REDasm
