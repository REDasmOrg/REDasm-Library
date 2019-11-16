#pragma once

#include <unordered_map>
#include <redasm/plugins/assembler/assembler.h>
#include <redasm/support/dispatcher.h>
#include <redasm/pimpl.h>
#include "../plugin_impl.h"

namespace REDasm {

class AssemblerImpl: public PluginImpl
{
    PIMPL_DECLARE_Q(Assembler)
    PIMPL_DECLARE_PUBLIC(Assembler)

    public:
        AssemblerImpl();
        void classify(Instruction *instruction) const;
        virtual void init(const AssemblerRequest &request);
        const AssemblerRequest &request() const;

    private:
        std::unordered_map<instruction_id_t, std::pair<InstructionType, InstructionFlags>> m_classifiedinstruction;
        Dispatcher<instruction_id_t, Instruction*> m_dispatcher;
        AssemblerRequest m_request;
};

} // namespace REDasm
