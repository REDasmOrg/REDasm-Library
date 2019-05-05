#pragma once

#include "../../redasm.h"
#include "../../support/serializer.h"

namespace REDasm {

typedef std::deque<address_t> ReferenceVector;
typedef std::set<address_t> ReferenceSet;

class ReferenceTable
{
    private:
        typedef std::unordered_map<address_t, ReferenceSet> ReferenceMap;

    public:
        ReferenceTable() = default;
        void push(address_t address, address_t refby);
        void pushTarget(address_t target, address_t pointedby);
        void popTarget(address_t target, address_t pointedby);
        ReferenceMap::const_iterator references(address_t address) const;
        ReferenceSet targets(address_t address) const;
        address_location target(address_t address) const;
        u64 referencesCount(address_t address) const;
        u64 targetsCount(address_t address) const;
        ReferenceVector referencesToVector(address_t address) const;

    public:
        static ReferenceVector toVector(const ReferenceSet& refset);

    private:
        ReferenceMap m_references;
        ReferenceMap m_targets;

    friend class Serializer<ReferenceTable>;
};

template<> struct Serializer<ReferenceTable> {
    static void write(std::fstream& fs, const ReferenceTable* t) {
        Serializer<ReferenceTable::ReferenceMap>::write(fs, t->m_references);
        Serializer<ReferenceTable::ReferenceMap>::write(fs, t->m_targets);
    }

    static void read(std::fstream& fs, ReferenceTable* t) {
        Serializer<ReferenceTable::ReferenceMap>::read(fs, t->m_references);
        Serializer<ReferenceTable::ReferenceMap>::read(fs, t->m_targets);
    }
};

} // namespace REDasm
