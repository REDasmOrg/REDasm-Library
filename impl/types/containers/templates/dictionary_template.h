#pragma once

#include <cstddef>

template<typename Container> class DictionaryTemplate
{
    public:
        typedef typename Container::key_type KeyType;
        typedef typename Container::mapped_type ValueType;

    public:
        DictionaryTemplate() = default;
        size_t size() const { return m_dict.size(); }
        const ValueType& value(const KeyType& key) const { return m_dict.at(key); }
        ValueType& value(const KeyType& key) { return m_dict[key]; }
        bool contains(const KeyType& key) const { return m_dict.find(key) != m_dict.end(); }
        void insert(const KeyType& key, const ValueType& value) { m_dict[key] = value; }
        void remove(const KeyType& key) { m_dict.erase(key); }
        void clear() { m_dict.clear(); }

    private:
        Container m_dict;
};
