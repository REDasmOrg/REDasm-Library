#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <deque>
#include <rdapi/types.h>

template<typename T> class AbstractContainer
{
    public:
        virtual ~AbstractContainer() = default;
        virtual T& at(size_t idx) = 0;
        virtual T& front() = 0;
        virtual T& back() = 0;
        virtual size_t insert(const T& t) = 0;
        virtual size_t indexOf(const T& t) const = 0;
        virtual size_t size() const  = 0;
        virtual bool empty() const = 0;
        virtual void remove(const T& t) = 0;
        virtual void removeAt(size_t idx) = 0;

    public:
        const T& front() const { return const_cast<AbstractContainer*>(this)->front(); }
        const T& back() const { return const_cast<AbstractContainer*>(this)->back(); }
        const T& at(size_t idx) const { return const_cast<AbstractContainer*>(this)->at(idx); }
        bool contains(const T& t) const { return this->indexOf(t) != RD_NPOS; }

        bool get(size_t idx, T* t) const {
            if(idx >= this->size()) return false;
            if(t) *t = this->at(idx);
            return true;
        }
};

template<typename T, typename Sorter = std::less<T>, typename Comparator = std::equal_to<T>, bool unique = false>
class SortedContainer: public AbstractContainer<T>
{
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
        void remove(const T& t) override;
        void removeAt(size_t idx) override;

    public:
        using AbstractContainer<T>::front;
        using AbstractContainer<T>::back;
        using AbstractContainer<T>::at;

    protected:
        std::vector<T> m_container;
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
    *t = m_container.data();
    return m_container.size();
}

template<typename T, typename Sorter, typename Comparator, bool unique>
size_t SortedContainer<T, Sorter, Comparator, unique>::insert(const T& t)
{
    if(unique) {
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
