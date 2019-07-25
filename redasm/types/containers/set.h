#pragma once

#include "container.h"

namespace REDasm {

class SetImpl;
class SortedSetImpl;

class Set: public SetContainer
{
    REDASM_OBJECT(Set)
    PIMPL_DECLARE_P(Set)
    PIMPL_DECLARE_PRIVATE(Set)

    public:
        Set();
        size_t size() const override;
        bool contains(const Variant &v) const override;
        void insert(const Variant &v) override;
        void remove(const Variant &v) override;
        void clear() override;

    public:
        Variant& at(size_t idx) override;
        Variant& first() override;
        Variant& last() override;
        const Variant& at(size_t idx) const override;
        const Variant& first() const override;
        const Variant& last() const override;
        size_t indexOf(const Variant &v) const override;
        void removeAt(size_t idx) override;
};

class SortedSet: public SetContainer
{
    REDASM_OBJECT(SortedSet)
    PIMPL_DECLARE_P(SortedSet)
    PIMPL_DECLARE_PRIVATE(SortedSet)

    public:
        SortedSet();
        size_t size() const override;
        bool contains(const Variant &v) const override;
        void insert(const Variant &v) override;
        void remove(const Variant &v) override;
        void clear() override;

    public:
        Variant& at(size_t idx) override;
        Variant& first() override;
        Variant& last() override;
        const Variant& at(size_t idx) const override;
        const Variant& first() const override;
        const Variant& last() const override;
        size_t indexOf(const Variant &v) const override;
        void removeAt(size_t idx) override;
};

} // namespace REDasm
