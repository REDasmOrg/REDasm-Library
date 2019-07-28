#pragma once

#include <functional>
#include <algorithm>
#include <deque>
#include <redasm/types/base.h>

namespace REDasm {

template<typename T> class ListTemplate
{
    public:
        typedef std::function<bool(const T&)> FindCallback;
        typedef std::function<void(const T&)> EachCallback;

    public:
        T& first() { return m_list.front(); }
        T& last() { return m_list.back(); }
        T& at(size_t idx) { return m_list.at(idx); }
        const T& first() const { return m_list.front(); }
        const T& last() const { return m_list.back(); }
        const T& at(size_t idx) const { return m_list.at(idx); }
        size_t size() const { return m_list.size(); }
        bool empty() const { return m_list.empty(); }
        void append(const T& v) { m_list.push_back(v); }
        void insert(size_t idx, const T& v) { m_list.insert(m_list.begin() + idx, v); }
        void eraseAt(size_t idx) { m_list.erase(m_list.begin() + idx); }
        void clear() { m_list.clear(); }

    public:
        size_t indexOf(const T& v) const {
            auto it = std::find(m_list.begin(), m_list.end(), v);
            return (it == m_list.end()) ? REDasm::npos : std::distance(m_list.begin(), it);
        }

        void remove(const T &v) {
            auto it = std::find(m_list.begin(), m_list.end(), v);

            if(it != m_list.end())
                m_list.erase(it);
        }

    public:
        inline size_t find(const FindCallback& cb) const { for(size_t i = 0; i < this->size(); i++) { if(cb(this->at(i))) return i; } return REDasm::npos; }
        inline void each(const EachCallback& cb) const { for(size_t i = 0; i < this->size(); i++) cb(this->at(i)); }

    protected:
        std::deque<T> m_list;
};

} // namespace REDasm
