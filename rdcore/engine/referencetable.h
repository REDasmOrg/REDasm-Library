#pragma once

#include <unordered_map>
#include <rdapi/types.h>
#include "../containers.h"

class ReferenceTable
{
    public:
        typedef SortedContainer<address_t, std::less<address_t>, std::equal_to<address_t>, true> References;
        typedef std::unordered_map<address_t, References> ReferencesMap;

    public:
        ReferenceTable() = default;
        void pushReference(address_t address, address_t refby);
        void popReference(address_t target, address_t refby);
        void pushTarget(address_t target, address_t refby);
        void popTarget(address_t target, address_t refby);
        RDLocation target(address_t address) const;
        size_t targets(address_t address, const address_t** targets) const;
        size_t references(address_t address, const address_t** references) const;
        size_t referencesCount(address_t address) const;
        size_t targetsCount(address_t address) const;

    private:
        ReferencesMap m_references, m_targets;
};
