#pragma once

#include <algorithm>
#include <deque>
#include <redasm/types/containers/list.h>
#include <redasm/types/variant.h>
#include "templates/sortedlist_template.h"

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

    public:
        template<typename Container> static void releaseObjects(Container& c) {
            for(auto it = c.begin(); it != c.end(); ) {
                if(it->isObject()) {
                    Object* obj = variant_object<Object>(*it);
                    it = c.erase(it);
                    obj->release();
                }
                else
                    it++;
            }
        }

    protected:
        std::deque<Variant> m_list;
};

class SortedListImpl
{
    PIMPL_DECLARE_Q(SortedList)
    PIMPL_DECLARE_PUBLIC(SortedList)

    public:
        SortedListImpl() = default;
        Variant& first();
        Variant& last();
        Variant& at(size_t idx);
        const Variant& first() const;
        const Variant& last() const;
        const Variant& at(size_t idx) const;
        size_t size() const;
        size_t indexOf(const Variant& obj) const;
        size_t insertionIndex(const Variant& v) const;
        void insert(const Variant& v);
        void removeAt(size_t idx);
        void remove(const Variant &v);
        void releaseObjects();
        void clear();

    protected:
        SortedListTemplate<Variant> m_list;
};

} // namespace REDasm
