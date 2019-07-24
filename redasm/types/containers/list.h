#pragma once

#include "container.h"

namespace REDasm {

class ListImpl;
class SortedListImpl;

class LIBREDASM_API List: public ListContainer
{
    REDASM_OBJECT(List)
    PIMPL_DECLARE_P(List)
    PIMPL_DECLARE_PRIVATE(List)

    public:
        List();
        const Variant& first() const override;
        const Variant& last() const override;
        const Variant& at(size_t idx) const override;
        Variant& first() override;
        Variant& last() override;
        Variant& at(size_t idx) override;
        size_t size() const override;
        size_t indexOf(const Variant &v) const override;
        void append(const Variant& v) override;
        void insert(size_t idx, const Variant& v) override;
        void removeAt(size_t idx) override;
        void remove(const Variant& v) override;
        void sort(const SortCallback& cb) override;
        void clear() override;

    public: // Special methods
        void save(cereal::BinaryOutputArchive& a) const override;
        void load(cereal::BinaryInputArchive& a) override;
        void releaseObjects();
};

class LIBREDASM_API SortedList: public ListContainerBase
{
    REDASM_OBJECT(SortedList)
    PIMPL_DECLARE_P(SortedList)
    PIMPL_DECLARE_PRIVATE(SortedList)

    public:
        SortedList();
        const Variant& first() const override;
        const Variant& last() const override;
        const Variant& at(size_t idx) const override;
        Variant& first() override;
        Variant& last() override;
        Variant& at(size_t idx) override;
        size_t size() const override;
        size_t indexOf(const Variant &v) const override;
        void removeAt(size_t idx) override;
        void remove(const Variant& v) override;
        void clear() override;

    public:
        size_t insertionIndex(const Variant& v) const;
        void insert(const Variant& v);

    public: // Special methods
        void save(cereal::BinaryOutputArchive& a) const override;
        void load(cereal::BinaryInputArchive& a) override;
        void releaseObjects();
};

} // namespace REDasm
