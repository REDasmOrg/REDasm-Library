#pragma once

#include <unordered_map>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/support/dispatcher.h>
#include <redasm/pimpl.h>
#include "../plugin_impl.h"

namespace REDasm {

class AssemblerImpl: public PluginImpl
{
    PIMPL_DECLARE_PUBLIC(Assembler)

    public:
        AssemblerImpl();
        void setInstructionType(const InstructionPtr& instruction) const;

    private:
        std::unordered_map<instruction_id_t, InstructionType> m_instructiontypes;
        Dispatcher<instruction_id_t, const InstructionPtr&> m_dispatcher;
};

} // namespace REDasm
