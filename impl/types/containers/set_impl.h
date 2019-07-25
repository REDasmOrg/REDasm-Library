#pragma once

#include <redasm/types/containers/set.h>
#include "templates/sortedlist_template.h"
#include <unordered_set>

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

    public:
        Variant& at(size_t idx);
        Variant& first();
        Variant& last();
        const Variant& at(size_t idx) const;
        const Variant& first() const;
        const Variant& last() const;
        size_t indexOf(const Variant &v) const;
        void removeAt(size_t idx);

    private:
        std::deque<Variant> m_list;
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

    public:
        Variant& at(size_t idx);
        Variant& first();
        Variant& last();
        const Variant& at(size_t idx) const;
        const Variant& first() const;
        const Variant& last() const;
        size_t indexOf(const Variant &v) const;
        void removeAt(size_t idx);

    private:
        SortedListTemplate<Variant> m_list;
        std::unordered_set<Variant> m_set;
};

} // namespace REDasm
