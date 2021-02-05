#pragma once

#include "abstractcontainer.h"
#include <algorithm>
#include <vector>

template<typename T, typename Comparator = std::equal_to<T>>
class FlatContainer: public RandomAccessContainer<T, std::vector<T>>
{
    public:
        FlatContainer() = default;
        size_t data(const T** t) const;
        void reserve(size_t n) { this->m_container.reserve(n); }

    public: // AbstractContainer Interface
        const T* insert(const T& t) override { this->m_container.push_back(t); return std::addressof(this->m_container.back()); }
        bool remove(const T& t) override;

    public: // RandomAccessContainer Interface
        size_t capacity() const override { return this->m_container.capacity(); }
        T& at(size_t idx) override { return this->m_container[idx]; }
        T& front() override { return this->m_container.front(); }
        T& back() override { return this->m_container.back(); }
        size_t indexOf(const T& t) const override;
        bool removeAt(size_t idx) override;

    public:
        using RandomAccessContainer<T, std::vector<T>>::front;
        using RandomAccessContainer<T, std::vector<T>>::back;
        using RandomAccessContainer<T, std::vector<T>>::at;
};

template<typename T, typename Comparator>
size_t FlatContainer<T, Comparator>::data(const T** t) const {
    if(t) *t = this->m_container.data();
    return this->m_container.size();
}

template<typename T, typename Comparator>
bool FlatContainer<T, Comparator>::remove(const T& t) {
    auto it = std::find_if(this->m_container.begin(), this->m_container.end(), [t](const T& item) {
        return Comparator{ }(t, item);
    });

    if(it == this->m_container.end()) return false;
    this->m_container.erase(it);
    return true;
}

template<typename T, typename Comparator>
size_t FlatContainer<T, Comparator>::indexOf(const T& t) const {
    auto it = std::find_if(this->m_container.begin(), this->m_container.end(), [t](const T& item) {
        return Comparator{ }(t, item);
    });

    return (it != this->m_container.end()) ? std::distance(this->m_container.begin(), it) : RD_NVAL;
}

template<typename T, typename Comparator>
bool FlatContainer<T, Comparator>::removeAt(size_t idx) {
    if(idx >= this->m_container.size()) return false;
    this->m_container.erase(std::next(this->m_container.begin(), idx));
    return true;
}
