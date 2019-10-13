#pragma once

#include <unordered_map>
#include <redasm/disassembler/listing/backend/referencetable.h>
#include <redasm/pimpl.h>

namespace REDasm {

class ReferenceTableImpl
{
    PIMPL_DECLARE_Q(ReferenceTable)
    PIMPL_DECLARE_PUBLIC(ReferenceTable)

    public:
        typedef std::unordered_map<address_t, SortedSet> ReferenceMap;

    public:
        ReferenceTableImpl() = default;
        void save(cereal::BinaryOutputArchive &a) const;
        void load(cereal::BinaryInputArchive &a);

    private:
        ReferenceMap m_references;
        ReferenceMap m_targets;
};

} // namespace REDasm
