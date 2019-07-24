#pragma once

#include <algorithm>
#include <deque>
#include <redasm/types/containers/list.h>
#include <redasm/types/variant.h>

namespace REDasm {

class ListImpl
{
    PIMPL_DECLARE_Q(List)
    PIMPL_DECLARE_PUBLIC(List)

    public:
        ListImpl() = default;
        Variant& first();
        Variant& last();
        Variant& at(size_t idx);
        const Variant& first() const;
        const Variant& last() const;
        const Variant& at(size_t idx) const;
        size_t size() const;
        size_t indexOf(const Variant& obj) const;
        void append(const Variant& obj);
        void insert(size_t idx, const Variant& v);
        void removeAt(size_t idx);
        void remove(const Variant &v);
        void releaseObjects();
        void clear();
        void sort(const List::SortCallback &cb);

    protected:
        std::deque<Variant> m_list;
};

} // namespace REDasm
