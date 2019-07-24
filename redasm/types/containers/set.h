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
};

} // namespace REDasm
