#pragma once

#include <redasm/disassembler/model/functiongraph.h>
#include <redasm/disassembler/model/calltree.h>

namespace REDasm {

class CallTreeImpl
{
    PIMPL_DECLARE_Q(CallTree)
    PIMPL_DECLARE_PUBLIC(CallTree)

    public:
        CallTreeImpl(CallTree* q);
        const FunctionGraph* graph() const;
        bool hasCalls() const;

    private:
        address_location fetchLocation() const;
};

} // namespace REDasm

