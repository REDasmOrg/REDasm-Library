#pragma once

#include <cstddef>
#include <algorithm>
#include <unordered_set>
#include <deque>
#include <redasm/types/base.h>

namespace REDasm {

template<typename T> class SetTemplate
{
    public:
        SetTemplate() = default;
        size_t size() const { return m_list.size(); }
        bool contains(const T &t) const { return m_set.find(t) != m_set.end(); }
        void insert(const T &t) { auto p = m_set.insert(t); if(p.second) m_list.push_back(t); }
        void remove(const T &t) { m_set.erase(t); m_list.erase(std::remove(m_list.begin(), m_list.end(), t)); }
        void clear() { m_set.clear(); m_list.clear(); }

    public:
        T& at(size_t idx) { return m_list[idx]; }
        T& first() { return m_list.front(); }
        T& last() { return m_list.back(); }
        const T& at(size_t idx) const { return m_list[idx]; }
        const T& first() const { return m_list.front(); }
        const T& last() const { return m_list.back(); }
        size_t indexOf(const T &t) const { auto it = std::find(m_list.begin(), m_list.end(), t); return (it == m_list.end()) ? REDasm::npos : std::distance(m_list.begin(), it); }
        void eraseAt(size_t idx) { m_set.erase(m_list[idx]); m_list.erase(m_list.begin() + idx); }

    private:
        std::deque<T> m_list;
        std::unordered_set<T> m_set;
};

} // namespace REDasm
