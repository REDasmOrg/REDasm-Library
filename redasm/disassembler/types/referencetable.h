#pragma once

#include "../../types/base_types.h"
#include "../../support/serializer.h"
#include "../../pimpl.h"

namespace REDasm {

class ReferenceTableImpl;

typedef std::deque<address_t> ReferenceVector;
typedef std::set<address_t> ReferenceSet;

class LIBREDASM_API ReferenceTable
{
    PIMPL_DECLARE_PRIVATE(ReferenceTable)

    public:
        typedef std::unordered_map<address_t, ReferenceSet> ReferenceMap;

    public:
        ReferenceTable();
        void push(address_t address, address_t refby);
        void pushTarget(address_t target, address_t pointedby);
        void popTarget(address_t target, address_t pointedby);
        ReferenceMap::const_iterator references(address_t address) const;
        ReferenceSet targets(address_t address) const;
        address_location target(address_t address) const;
        size_t referencesCount(address_t address) const;
        size_t targetsCount(address_t address) const;
        ReferenceVector referencesToVector(address_t address) const;

    public:
        static ReferenceVector toVector(const ReferenceSet& refset);

    friend struct Serializer<ReferenceTable>;
};

template<> struct Serializer<ReferenceTable> {
    static void write(std::fstream& fs, const ReferenceTable* t);
    static void read(std::fstream& fs, ReferenceTable* t);
};

} // namespace REDasm
