#pragma once

#include <unordered_map>
#include <map>
#include "abstractcontainer.h"

template<typename K, typename V, typename Container>
class AbstractDictContainer: public AbstractContainer<std::pair<K, V>, Container>
{
    public:
        typedef std::pair<K, V> Item;
        typedef K Key;
        typedef V Value;

    public:
        const Item* insert(const Item& item) override {
            auto it = this->m_container.insert({ item.first, item.second });
            return std::addressof(*it);
        }

        bool get(const K& k, V* v) const {
            auto it = this->m_container.find(k);
            if(it == this->m_container.end()) return false;

            if(v) *v = it.second;
            return true;
        }

        bool remove(const Item& item) override {
            auto it = this->m_container.find(item.first);
            if(it == this->m_container.end()) return false;
            this->m_container.erase(it);
            return true;
        }

        bool remove(const K& k) { return this->m_container.erase(k); }
        bool contains(const K& k) const { return this->m_container.count(k); }
};

template<typename K, typename V>
using DictContainer = AbstractDictContainer<K, V, std::unordered_map<K, V>>;

template<typename K, typename V>
using SortedDictContainer = AbstractDictContainer<K, V, std::map<K, V>>;
