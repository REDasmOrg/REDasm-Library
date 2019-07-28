#pragma once

#include "container.h"

namespace REDasm {

class DictionaryImpl;
class SortedDictionaryImpl;

class LIBREDASM_API Dictionary: public DictionaryContainer
{
    REDASM_OBJECT(Dictionary)
    PIMPL_DECLARE_P(Dictionary)
    PIMPL_DECLARE_PRIVATE(Dictionary)

    public:
        Dictionary();
        size_t size() const override;
        const ListContainerBase& keys() const override;
        const Variant& value(const Variant& key) const override;
        Variant& value(const Variant& key) override;
        bool contains(const Variant& key) const override;
        void insert(const Variant& key, const Variant& value) override;
        void remove(const Variant& key) override;
        void clear() override;
        const Variant& operator[](const Variant& key) const;
        Variant& operator[](const Variant& key);
};

class LIBREDASM_API SortedDictionary: public DictionaryContainer
{
    REDASM_OBJECT(SortedDictionary)
    PIMPL_DECLARE_P(SortedDictionary)
    PIMPL_DECLARE_PRIVATE(SortedDictionary)

    public:
        SortedDictionary();
        size_t size() const override;
        const ListContainerBase& keys() const override;
        const Variant& value(const Variant& key) const override;
        Variant& value(const Variant& key) override;
        bool contains(const Variant& key) const override;
        void insert(const Variant& key, const Variant& value) override;
        void remove(const Variant& key) override;
        void clear() override;
        const Variant& operator[](const Variant& key) const;
        Variant& operator[](const Variant& key);
};

} // namespace REDasm
