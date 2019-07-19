#pragma once

#include <redasm/disassembler/types/referencetable.h>
#include <redasm/pimpl.h>

namespace REDasm {

class ReferenceTableImpl
{
    PIMPL_DECLARE_Q(ReferenceTable)
    PIMPL_DECLARE_PUBLIC(ReferenceTable)

    public:
        ReferenceTableImpl() = default;
        void save(cereal::BinaryOutputArchive &a) const;
        void load(cereal::BinaryInputArchive &a);

    private:
        ReferenceTable::ReferenceMap m_references;
        ReferenceTable::ReferenceMap m_targets;
};

} // namespace REDasm
