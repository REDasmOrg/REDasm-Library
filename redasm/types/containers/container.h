#pragma once

#include <functional>
#include "../object.h"
#include "../../macros.h"
#include "../../pimpl.h"

namespace REDasm {

class LIBREDASM_API Container: public Object
{
    public:
        Container() = default;
        bool empty() const;
        virtual size_t size() const = 0;
        virtual void clear() = 0;
};

class LIBREDASM_API ListContainerBase: public Container
{
    public:
        typedef bool(*SortCallback)(const Variant&, const Variant&);

    public:
        void removeFirst();
        void removeLast();
        const Variant& operator[](size_t idx) const;
        Variant& operator[](size_t idx);

    public:
        virtual Variant& at(size_t idx) = 0;
        virtual Variant& first() = 0;
        virtual Variant& last() = 0;
        virtual const Variant& at(size_t idx) const = 0;
        virtual const Variant& first() const = 0;
        virtual const Variant& last() const = 0;
        virtual size_t indexOf(const Variant &v) const = 0;
        virtual void removeAt(size_t idx) = 0;
        virtual void remove(const Variant& v) = 0;

    public:
        inline size_t find(const std::function<bool(const Variant&)>& cb) const { for(size_t i = 0; i < this->size(); i++) { if(cb(this->at(i))) return i; } return REDasm::npos; }
        inline void each(const std::function<void(const Variant&)>& cb) const { for(size_t i = 0; i < this->size(); i++) cb(this->at(i)); }
};

class LIBREDASM_API ListContainer: public ListContainerBase
{
    public:
        virtual void append(const Variant& v) = 0;
        virtual void insert(size_t idx, const Variant& v) = 0;
        virtual void sort(const SortCallback& cb) = 0;
};

class LIBREDASM_API SetContainer: public Container
{
    public:
        virtual bool contains(const Variant& v) const = 0;
        virtual void insert(const Variant& v) = 0;
        virtual void remove(const Variant& v) = 0;
};

class LIBREDASM_API DictionaryContainer: public Container
{
    public:
        const Variant& operator[](const Variant& key) const;
        Variant& operator[](const Variant& key);

    public:
        virtual const Variant& value(const Variant& key) const = 0;
        virtual Variant& value(const Variant& key) = 0;
        virtual bool contains(const Variant& key) const = 0;
        virtual void insert(const Variant& key, const Variant& value) = 0;
        virtual void remove(const Variant& key) = 0;
};

} // namespace REDasm
