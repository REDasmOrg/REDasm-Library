#pragma once

#include <redasm/types/containers/container.h>
#include <cstddef>

namespace REDasm {

template<typename STLContainer, typename KeysContainer> class DictionaryTemplate
{
    public:
        typedef typename STLContainer::key_type KeyType;
        typedef typename STLContainer::mapped_type ValueType;

    public:
        DictionaryTemplate() = default;
        bool empty() const { return m_dict.empty(); }
        size_t size() const { return m_dict.size(); }
        const KeyType& first() const { return m_dict.begin()->first; }
        const KeyType& last() const { return m_dict.rbegin()->first; }
        const ListContainerBase& keys() const { return m_keys; }
        const ValueType& value(const KeyType& key) const { return m_dict.at(key); }
        ValueType& value(const KeyType& key) { m_keys.insert(key); return m_dict[key]; }
        bool contains(const KeyType& key) const { return m_keys.contains(key); }
        void insert(const KeyType& key, const ValueType& value) { m_keys.insert(key); m_dict[key] = value; }
        void erase(const KeyType& key) { m_keys.erase(key); m_dict.erase(key); }
        void clear() { m_keys.clear(); m_dict.clear(); }

    private:
        KeysContainer m_keys;
        STLContainer m_dict;
};

} // namespace REDasm
