#pragma once

#include <redasm/types/containers/dictionary.h>
#include <unordered_map>
#include <map>

namespace REDasm {

class DictionaryImpl
{
    public:
        DictionaryImpl() = default;
        size_t size() const;
        const Variant& value(const Variant& key) const;
        Variant& value(const Variant& key);
        bool contains(const Variant& key) const;
        void insert(const Variant& key, const Variant& value);
        void remove(const Variant& key);
        void clear();

    private:
        std::unordered_map<Variant, Variant> m_dict;
};

class SortedDictionaryImpl
{
    public:
        SortedDictionaryImpl() = default;
        size_t size() const;
        const Variant& value(const Variant& key) const;
        Variant& value(const Variant& key);
        bool contains(const Variant& key) const;
        void insert(const Variant& key, const Variant& value);
        void remove(const Variant& key);
        void clear();

    private:
        std::map<Variant, Variant> m_dict;
};

} // namespace REDasm
