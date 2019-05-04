#pragma once

#include "../../redasm.h"
#include "../../support/serializer.h"

namespace REDasm {

typedef std::deque<address_t> ReferenceVector;
typedef std::set<address_t> ReferenceSet;

class ReferenceTable: public Serializer::Serializable
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
        void serializeTo(std::fstream& fs) override;
        void deserializeFrom(std::fstream& fs) override;

    private:
        void serializeMap(const ReferenceMap& rm, std::fstream& fs);
        void deserializeMap(ReferenceMap& rm, std::fstream& fs);

    public:
        static ReferenceVector toVector(const ReferenceSet& refset);

    private:
        ReferenceMap m_references;
        ReferenceMap m_targets;
};

}
