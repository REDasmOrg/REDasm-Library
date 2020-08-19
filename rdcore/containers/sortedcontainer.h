#pragma once

#include <functional>
#include <vector>
#include "abstractcontainer.h"

template<typename T, typename Sorter = std::less<T>, typename Comparator = std::equal_to<T>, bool unique = false>
class SortedContainer: public AbstractContainer<T>
{
    private:
        typedef std::vector<T> ContainerType;

    public:
        SortedContainer() = default;
        size_t data(const T** t) const;
        T& at(size_t idx) override { return m_container[idx]; }
        T& front() override { return m_container.front(); }
        T& back() override { return m_container.back(); }
        size_t insert(const T& t) override;
        size_t indexOf(const T& t) const override;
        size_t size() const override { return m_container.size(); }
        size_t capacity() { return m_container.capacity(); }
        bool empty() const override { return m_container.empty(); }
        void reserve(size_t n) { m_container.reserve(n); }
        void remove(const T& t) override;
        void removeAt(size_t idx) override;
        void clear() override;

    public:
        typename ContainerType::iterator begin() { return m_container.begin(); }
        typename ContainerType::iterator end() { return m_container.end(); }
        typename ContainerType::const_iterator begin() const { return m_container.begin(); }
        typename ContainerType::const_iterator end() const { return m_container.end(); }

    public:
        using AbstractContainer<T>::front;
        using AbstractContainer<T>::back;
        using AbstractContainer<T>::at;

    protected:
        ContainerType m_container;
};

template<typename T, typename Sorter, typename Comparator, bool unique>
size_t SortedContainer<T, Sorter, Comparator, unique>::indexOf(const T& t) const
{
    auto it = std::lower_bound(m_container.begin(), m_container.end(), t, Sorter());
    if(it == m_container.end()) return RD_NPOS;
    if(!Comparator()(*it, t)) return RD_NPOS;
    return std::distance(m_container.begin(), it);
}

template<typename T, typename Sorter, typename Comparator, bool unique>
void SortedContainer<T, Sorter, Comparator, unique>::remove(const T& t)
{
    size_t idx = this->indexOf(t);
    if(idx != RD_NPOS) this->removeAt(idx);
}

template<typename T, typename Sorter, typename Comparator, bool unique>
size_t SortedContainer<T, Sorter, Comparator, unique>::data(const T** t) const
{
    if(t) *t = m_container.data();
    return m_container.size();
}

template<typename T, typename Sorter, typename Comparator, bool unique>
size_t SortedContainer<T, Sorter, Comparator, unique>::insert(const T& t)
{
    if constexpr(unique) {
        size_t idx = this->indexOf(t);
        if(idx != RD_NPOS) return idx;
    }

    auto it = std::upper_bound(m_container.begin(), m_container.end(), t, Sorter());
    return std::distance(m_container.begin(), m_container.insert(it, t));
}

template<typename T, typename Sorter, typename Comparator, bool unique>
void SortedContainer<T, Sorter, Comparator, unique>::removeAt(size_t idx)
{
    if(idx >= m_container.size()) return;
    m_container.erase(std::next(m_container.begin(), idx));
}

template<typename T, typename Sorter, typename Comparator, bool unique>
void SortedContainer<T, Sorter, Comparator, unique>::clear()
{
    m_container.clear();
}
