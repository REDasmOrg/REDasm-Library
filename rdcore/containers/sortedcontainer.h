#pragma once

#include "flatcontainer.h"

template<typename T, typename Comparator = std::equal_to<T>, typename Sorter = std::less<T>, bool unique = false>
class SortedContainer: public FlatContainer<T, Comparator>
{
    public:
        SortedContainer() = default;

    public: // AbstractContainer Interface
        const T* insert(const T& t) override;
        bool remove(const T& t) override;

    public: // RandomAccessContainer Interface
        size_t indexOf(const T& t) const override;

    public:
        using FlatContainer<T, Comparator>::front;
        using FlatContainer<T, Comparator>::back;
        using FlatContainer<T, Comparator>::at;
};

template<typename T, typename Comparator, typename Sorter, bool unique>
size_t SortedContainer<T, Comparator, Sorter, unique>::indexOf(const T& t) const {
    auto it = std::lower_bound(this->begin(), this->end(), t, Sorter());
    if(it == this->end()) return RD_NVAL;
    if(!Comparator()(*it, t)) return RD_NVAL;
    return std::distance(this->begin(), it);
}

template<typename T, typename Comparator, typename Sorter, bool unique>
bool SortedContainer<T, Comparator, Sorter, unique>::remove(const T& t) {
    size_t idx = this->indexOf(t);
    return (idx != RD_NVAL) ? this->removeAt(idx) : false;
}

template<typename T, typename Comparator, typename Sorter, bool unique>
const T* SortedContainer<T, Comparator, Sorter, unique>::insert(const T& t) {
    if constexpr(unique) {
        size_t idx = this->indexOf(t);
        if(idx != RD_NVAL) return std::addressof(this->m_container[idx]);
    }

    auto it = std::upper_bound(this->begin(), this->end(), t, Sorter());
    return std::addressof(*this->m_container.insert(it, t));
}
