#pragma once

#include <redasm/disassembler/types/referencetable.h>
#include <redasm/pimpl.h>

namespace REDasm {

class ReferenceTableImpl
{
    PIMPL_DECLARE_PUBLIC(ReferenceTable)

    public:
        ReferenceTableImpl() = default;

    private:
        ReferenceTable::ReferenceMap m_references;
        ReferenceTable::ReferenceMap m_targets;

    friend struct Serializer<ReferenceTable>;
};

} // namespace REDasm
