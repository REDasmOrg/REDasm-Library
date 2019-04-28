#pragma once

#include "../../support/containers/cache_map.h"
#include "../../redasm.h"

namespace REDasm {

class InstructionCache: public cache_map<address_t, InstructionPtr>
{
    public:
        InstructionCache();
        void update(const InstructionPtr &instruction);

    protected:
        virtual void serialize(const InstructionPtr &value, std::fstream &fs);
        virtual void deserialize(InstructionPtr &value, std::fstream &fs);
};

} // namespace REDasm
