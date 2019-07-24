#pragma once

#include <deque>
#include <set>
#include "../../types/object.h"
#include "../../macros.h"
#include "../../pimpl.h"

namespace REDasm {

class ReferenceTableImpl;

typedef std::deque<address_t> ReferenceVector;
typedef std::set<address_t> ReferenceSet;

class LIBREDASM_API ReferenceTable: public Object
{
    REDASM_OBJECT(ReferenceTable)
    PIMPL_DECLARE_P(ReferenceTable)
    PIMPL_DECLARE_PRIVATE(ReferenceTable)

    public:
        void save(cereal::BinaryOutputArchive &a) const override;
        void load(cereal::BinaryInputArchive &a) override;

    public:
        ReferenceTable();
        void push(address_t address, address_t refby);
        void pushTarget(address_t target, address_t pointedby);
        void popTarget(address_t target, address_t pointedby);
        ReferenceSet targets(address_t address) const;
        address_location target(address_t address) const;
        size_t referencesCount(address_t address) const;
        size_t targetsCount(address_t address) const;
        ReferenceVector referencesToVector(address_t address) const;

    public:
        static ReferenceVector toVector(const ReferenceSet& refset);
};

} // namespace REDasm
