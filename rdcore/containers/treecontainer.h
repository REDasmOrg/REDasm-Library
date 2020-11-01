#pragma once

#include <set>
#include "abstractcontainer.h"

template<typename T, typename Sorter = std::less<T>, typename Container = std::set<T, Sorter>>
class TreeContainer: public AbstractContainer<T, Container>
{
    public:
        TreeContainer() = default;
        void range(const T& first, const T& last, const typename TreeContainer::TypeCallback& cb) const;

    public:
        template<typename K> bool contains(const K& k) const {
            if(this->m_container.empty()) return false;
            return this->get(k, nullptr);
        }

        template<typename K> bool get(const K& k, T* t) const {
            auto it = this->m_container.find(k);
            if(it == this->m_container.end()) return false;

            if(t) *t = *it;
            return true;
        }

    public: // Internal C++ Helpers
        template<typename K> typename TreeContainer::Iterator find(const K& k) { return this->m_container.find(k); }
        template<typename K> typename TreeContainer::ConstIterator find(const K& k) const { return this->m_container.find(k); }
        template<typename K> typename TreeContainer::Iterator lowerBound(const K& k) { return this->m_container.lowerBound(k); }
        template<typename K> typename TreeContainer::ConstIterator lowerBound(const K& k) const { return this->m_container.lower_bound(k); }
        template<typename K> typename TreeContainer::Iterator upperBound(const K& k) { return this->m_container.upperBound(k); }
        template<typename K> typename TreeContainer::ConstIterator upperBound(const K& k) const { return this->m_container.upper_bound(k); }

    public: // AbstractContainer Interface
        bool remove(const T& t) override;
        const T* insert(const T& t) override;
};

template<typename T, typename Sorter, typename Container>
void TreeContainer<T, Sorter, Container>::range(const T& first, const T& last, const typename TreeContainer<T, Sorter, Container>::TypeCallback& cb) const {
    auto startit = this->m_container.find(first);
    if(startit == this->m_container.end()) return;

    auto endit = this->m_container.find(last);
    if(endit == this->m_container.end()) return;
    endit++;

    for(auto it = startit; (it != this->m_container.end()) && (it != endit); it++) {
        if(!cb(*it)) break;
    }
}

template<typename T, typename Sorter, typename Container>
bool TreeContainer<T, Sorter, Container>::remove(const T& t) { return this->m_container.erase(t) > 0; }

template<typename T, typename Sorter, typename Container>
const T* TreeContainer<T, Sorter, Container>::insert(const T& t) {
    auto res = this->m_container.insert(t);

    if constexpr(std::is_same<Container, std::set<T, Sorter>>::value) return std::addressof(*res.first);
    else return std::addressof(*res);
}

template<typename T, typename Sorter = std::less<T>>
class MultiTreeContainer: public TreeContainer<T, Sorter, std::multiset<T, Sorter>>
{
    public:
        MultiTreeContainer() = default;
};
