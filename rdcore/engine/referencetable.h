#pragma once

#include <unordered_map>
#include <rdapi/types.h>
#include "../containers.h"

class ReferenceTable
{
    public:
        typedef SortedContainer<rd_address, std::less<rd_address>, std::equal_to<rd_address>, true> References;
        typedef std::unordered_map<rd_address, References> ReferencesMap;

    public:
        ReferenceTable() = default;
        void pushReference(rd_address address, rd_address refby);
        void popReference(rd_address target, rd_address refby);
        void pushTarget(rd_address target, rd_address refby);
        void popTarget(rd_address target, rd_address refby);
        RDLocation target(rd_address address) const;
        size_t targets(rd_address address, const rd_address** targets) const;
        size_t references(rd_address address, const rd_address** references) const;
        size_t referencesCount(rd_address address) const;
        size_t targetsCount(rd_address address) const;

    private:
        ReferencesMap m_references, m_targets;
};
