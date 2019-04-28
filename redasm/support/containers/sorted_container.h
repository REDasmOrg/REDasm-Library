#pragma once

#include <functional>
#include <algorithm>
#include <deque>

namespace REDasm {

template< typename T, typename Comparator = std::less<T>, typename Container = std::deque<T> > class sorted_container
{
    public:
        static const size_t npos = -1;
        typedef typename Container::iterator iterator;
        typedef typename Container::const_iterator const_iterator;
        typedef typename Container::reverse_iterator reverse_iterator;
        typedef typename Container::const_reverse_iterator const_reverse_iterator;

    public:
        sorted_container() = default;
        size_t size() const { return m_container.size(); }
        bool empty() const { return m_container.empty(); }
        iterator begin() { return m_container.begin(); }
        iterator end() { return m_container.end(); }
        reverse_iterator rbegin() { return m_container.rbegin(); }
        reverse_iterator rend() { return m_container.rend(); }
        const_iterator begin() const { return m_container.begin(); }
        const_iterator end() const { return m_container.end(); }
        const_iterator cbegin() const { return m_container.cbegin(); }
        const_iterator cend() const { return m_container.cend(); }
        const_reverse_iterator rbegin() const { return m_container.rbegin(); }
        const_reverse_iterator rend() const { return m_container.rend(); }
        const T& at(size_t idx) const { return m_container.at(idx); }
        T& at(size_t idx) { return m_container.at(idx); }
        const T& front() const { return m_container.front(); }
        T& front() { return m_container.front(); }
        const T& back() const { return m_container.back(); }
        T& back() { return m_container.back(); }

        size_t indexOf(const T& t) const {
            auto it = this->find(t);
            return (it == this->end()) ? npos : std::distance(this->begin(), it);
        }

        template<typename CustomComparator> size_t indexOf(const T& t, const CustomComparator& comparator) const {
            auto it = this->find(t, comparator);
            return (it == this->end()) ? npos : std::distance(this->begin(), it);
        }

        size_t insertionIndex(const T& t) const {
            auto it = std::lower_bound(m_container.begin(), m_container.end(), t, Comparator());
            return std::distance(this->begin(), it);
        }

        template<typename Iterator> Iterator erase(const Iterator& it) { return m_container.erase(it); }
        iterator erase(size_t idx) { return m_container.erase(std::next(this->begin(), idx)); }
        iterator erase(const T& t) { return this->erase(this->find(t)); }
        const T& operator[](size_t idx) const { return m_container[idx]; }
        T& operator[](size_t idx) { return m_container[idx]; }

        typename Container::const_iterator find(const T& t) const {
            auto it = std::lower_bound(m_container.begin(), m_container.end(), t, Comparator());
            return (it != m_container.end() && !Comparator()(t, *it)) ? it : m_container.end();
        }

        template<typename CustomComparator> typename Container::const_iterator find(const T& t, const CustomComparator& comparator) const {
            auto it = std::lower_bound(m_container.begin(), m_container.end(), t, comparator);
            return (it != m_container.end() && !comparator(t, *it)) ? it : m_container.end();
        }

        typename Container::iterator find(const T& t) {
            auto it = std::lower_bound(m_container.begin(), m_container.end(), t, Comparator());
            return (it != m_container.end() && !Comparator()(t, *it)) ? it : m_container.end();
        }

        template<typename CustomComparator> typename Container::iterator find(const T& t, const CustomComparator& comparator) {
            auto it = std::lower_bound(m_container.begin(), m_container.end(), t, comparator);
            return (it != m_container.end() && !comparator(t, *it)) ? it : m_container.end();
        }

        typename Container::iterator insert(const T& t) {
            auto it = std::lower_bound(m_container.begin(), m_container.end(), t, Comparator());
            return m_container.insert(it, t);
        }

        typename Container::iterator insert(T&& t) {
            auto it = std::lower_bound(m_container.begin(), m_container.end(), t, Comparator());
            return m_container.insert(it, std::move(t));
        }

    private:
        Container m_container;
};

} // namespace REDasm
