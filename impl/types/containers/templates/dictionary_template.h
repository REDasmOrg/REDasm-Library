#pragma once

#include <redasm/types/containers/container.h>
#include <cstddef>

namespace REDasm {

template<typename Container, typename KeysContainer> class DictionaryTemplate
{
    public:
        typedef typename Container::key_type KeyType;
        typedef typename Container::mapped_type ValueType;

    public:
        DictionaryTemplate() = default;
        size_t size() const { return m_dict.size(); }
        const ListContainerBase& keys() const { return m_keys; }
        const ValueType& value(const KeyType& key) const { return m_dict.at(key); }
        ValueType& value(const KeyType& key) { m_keys.insert(key); return m_dict[key]; }
        bool contains(const KeyType& key) const { return m_keys.contains(key); }
        void insert(const KeyType& key, const ValueType& value) { m_keys.insert(key); m_dict[key] = value; }
        void remove(const KeyType& key) { m_keys.remove(key); m_dict.erase(key); }
        void clear() { m_keys.clear(); m_dict.clear(); }

    private:
        KeysContainer m_keys;
        Container m_dict;
};

} // namespace REDasm
