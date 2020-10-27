#pragma once

#include <functional>
#include "basecontainer.h"

template<typename T, typename Comparator = std::equal_to<T>>
class UniqueContainer: public FlatContainer<T, Comparator>
{
    public:
        UniqueContainer() = default;

    public: // AbstractContainer Interface
        const T* insert(const T& t) override;
};

template<typename T, typename Comparator>
const T* UniqueContainer<T, Comparator>::insert(const T& t) {
    size_t idx = this->indexOf(t);
    if(idx != RD_NPOS) return std::addressof(this->m_container[idx]);
    return FlatContainer<T, Comparator>::insert(t);
}
