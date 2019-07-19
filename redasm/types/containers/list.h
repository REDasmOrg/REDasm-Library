#pragma once

#include "../object.h"
#include "../variant.h"
#include "../../pimpl.h"

namespace REDasm {

class ListImpl;
class ListIteratorImpl;
class List;

class ListIterator
{
    PIMPL_DECLARE_P(ListIterator)
    PIMPL_DECLARE_PRIVATE(ListIterator)

    protected:
        ListIterator(List* list, size_t startidx);

    public:
        bool hasNext() const;
        size_t index() const;
        Variant next();
        void remove();

    friend class List;
};

class ListConstIterator: public ListIterator
{
    private:
        ListConstIterator(List* list, size_t startidx);
        void remove() = delete;

    friend class List;
};

class List: public Object
{
    REDASM_OBJECT(List)
    PIMPL_DECLARE_P(List)
    PIMPL_DECLARE_PRIVATE(List)

    public:
        List();
        ListConstIterator iterator(size_t idx = 0) const;
        ListIterator iterator(size_t idx = 0);
        Variant& first();
        const Variant& first() const;
        Variant& last();
        const Variant& last() const;
        Variant& at(size_t idx);
        const Variant& at(size_t idx) const;
        size_t size() const;
        size_t indexOf(const Variant &v) const;
        bool empty() const;
        void append(const Variant& v);
        void insert(size_t idx, const Variant& v);
        void removeAt(size_t idx);
        void remove(const Variant& v);
        void removeFirst();
        void removeLast();
        void sort(bool(*cb)(const Variant&, const Variant&));
        Variant& operator[](size_t idx);
        const Variant& operator[](size_t idx) const;

    public: // Special methods
        void save(cereal::BinaryOutputArchive& a) const override;
        void load(cereal::BinaryInputArchive& a) override;
        void releaseObjects();

};

} // namespace REDasm
