#pragma once

#include <redasm/types/containers/set.h>
#include <unordered_set>
#include <set>

namespace REDasm {

class SetImpl
{
    public:
        SetImpl() = default;
        size_t size() const;
        bool contains(const Variant &v) const;
        void insert(const Variant &v);
        void remove(const Variant &v);
        void clear();

    private:
        std::unordered_set<Variant> m_set;
};

class SortedSetImpl
{
    public:
        SortedSetImpl() = default;
        size_t size() const;
        bool contains(const Variant &v) const;
        void insert(const Variant &v);
        void remove(const Variant &v);
        void clear();

    private:
        std::set<Variant> m_set;
};

} // namespace REDasm
