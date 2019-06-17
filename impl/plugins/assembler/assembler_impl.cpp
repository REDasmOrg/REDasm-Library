#include "assembler_impl.h"
#include <redasm/plugins/assembler/algorithm/algorithm.h>

namespace REDasm {

AssemblerImpl::AssemblerImpl(): PluginImpl() { }

void AssemblerImpl::setInstructionType(Instruction* instruction) const
{
    auto it = m_instructiontypes.find(instruction->id());

    if(it != m_instructiontypes.end())
        instruction->type() |= it->second;
}

void AssemblerImpl::init(const AssemblerRequest& request) { m_request = request; }
const AssemblerRequest& AssemblerImpl::request() const { return m_request; }

} // namespace REDasm
