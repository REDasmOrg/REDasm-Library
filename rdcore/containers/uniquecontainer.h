#pragma once

#include <functional>
#include "abstractcontainer.h"

template<typename T, typename Comparator = std::equal_to<T>>
class UniqueContainer: public AbstractContainer<T>
{
    private:
        typedef std::vector<T> ContainerType;

    public:
        UniqueContainer() = default;
        size_t data(const T** t) const;
        T& at(size_t idx) override { return m_container[idx]; }
        T& front() override { return m_container.front(); }
        T& back() override { return m_container.back(); }
        size_t insert(const T& t) override;
        size_t indexOf(const T& t) const override;
        size_t size() const override { return m_container.size(); }
        bool empty() const override { return m_container.empty(); }
        void remove(const T& t) override;
        void removeAt(size_t idx) override;
        void clear() override { m_container.clear(); }

    public:
        using AbstractContainer<T>::front;
        using AbstractContainer<T>::back;
        using AbstractContainer<T>::at;

    public: // Ranged-For support
        typename ContainerType::iterator begin() { return m_container.begin(); }
        typename ContainerType::iterator end() { return m_container.end(); }
        typename ContainerType::const_iterator begin() const { return m_container.begin(); }
        typename ContainerType::const_iterator end() const { return m_container.end(); }

    private:
        ContainerType m_container;
};

template<typename T, typename Comparator>
size_t UniqueContainer<T, Comparator>::data(const T** t) const
{
    if(t) *t = m_container.data();
    return m_container.size();
}

template<typename T, typename Comparator>
size_t UniqueContainer<T, Comparator>::insert(const T& t)
{
    size_t idx = this->indexOf(t);
    if(idx != RD_NPOS) return idx;
    m_container.push_back(t);
    return m_container.size() - 1;
}

template<typename T, typename Comparator>
size_t UniqueContainer<T, Comparator>::indexOf(const T& t) const
{
    auto it = std::find(m_container.begin(), m_container.end(), t);
    return (it != m_container.end()) ? std::distance(m_container.begin(), it) : RD_NPOS;
}

template<typename T, typename Comparator>
void UniqueContainer<T, Comparator>::remove(const T& t)
{
    size_t idx = this->indexOf(t);
    if(idx != RD_NPOS) this->removeAt(idx);
}

template<typename T, typename Comparator>
void UniqueContainer<T, Comparator>::removeAt(size_t idx)
{
    if(idx >= m_container.size()) return;
    m_container.erase(std::next(m_container.begin(), idx));
}
