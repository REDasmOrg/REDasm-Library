#pragma once

#include "sortedlist_template.h"
#include <unordered_set>

namespace REDasm {

template<typename T> class SortedSetTemplate
{
    public:
        SortedSetTemplate() = default;
        size_t size() const { return m_list.size(); }
        bool contains(const T& t) const { return m_set.find(t) != m_set.end(); }
        void insert(const T& t) { auto p = m_set.insert(t); if(p.second) m_list.insert(t); }
        void remove(const T& t) { m_set.erase(t); m_list.erase(t); }
        void clear() { m_set.clear(); m_list.clear(); }

    public:
        T& at(size_t idx) { return m_list[idx]; }
        T& first() { return m_list.front(); }
        T& last() { return m_list.back(); }
        const T& at(size_t idx) const { return m_list[idx];}
        const T& first() const { return m_list.front(); }
        const T& last() const { return m_list.back(); }
        size_t indexOf(const T& t) const { return m_list.indexOf(t); }
        void removeAt(size_t idx) { m_list.eraseAt(idx); }

    private:
        SortedListTemplate<T> m_list;
        std::unordered_set<T> m_set;
};

} // namespace REDasm
