#include "assembler_impl.h"
#include <redasm/plugins/assembler/algorithm/algorithm.h>

namespace REDasm {

AssemblerImpl::AssemblerImpl(): PluginImpl() { }

void AssemblerImpl::classify(Instruction* instruction) const
{
    auto it = m_classifiedinstruction.find(instruction->id);
    if(it == m_classifiedinstruction.end()) return;

    instruction->type = it->second.first;
    instruction->flags |= it->second.second;
}

void AssemblerImpl::init(const AssemblerRequest& request) { m_request = request; }
const AssemblerRequest& AssemblerImpl::request() const { return m_request; }

} // namespace REDasm
