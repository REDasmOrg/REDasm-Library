#pragma once

#include <algorithm>
#include <deque>
#include <redasm/types/containers/list.h>
#include <redasm/types/variant.h>

namespace REDasm {

class ListIteratorImpl
{
    public:
        ListIteratorImpl(List* list, size_t startidx);
        bool hasNext() const;
        size_t index() const;
        Variant next();
        void remove();

    private:
        List* m_list;
        size_t m_index;
};

class ListImpl
{
    PIMPL_DECLARE_Q(List)
    PIMPL_DECLARE_PUBLIC(List)

    public:
        ListImpl() = default;
        Variant& first();
        Variant& last();
        const Variant& first() const;
        const Variant& last() const;
        Variant& at(size_t idx);
        const Variant& at(size_t idx) const;
        size_t size() const;
        size_t indexOf(const Variant& obj) const;
        bool empty() const;
        void append(const Variant& obj);
        void insert(size_t idx, const Variant& v);
        void removeAt(size_t idx);
        void remove(const Variant &v);
        void removeFirst();
        void removeLast();
        void releaseObjects();
        void sort(bool(*cb)(const Variant&, const Variant&));

    protected:
        std::deque<Variant> m_list;
};



} // namespace REDasm
