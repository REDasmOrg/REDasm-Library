#pragma once

#include <cstddef>
#include <rdapi/types.h>

template<typename T> class AbstractContainer
{
    public:
        virtual ~AbstractContainer() = default;
        virtual T& at(size_t idx) = 0;
        virtual T& front() = 0;
        virtual T& back() = 0;
        virtual size_t insert(const T& t) = 0;
        virtual size_t indexOf(const T& t) const = 0;
        virtual size_t size() const  = 0;
        virtual bool empty() const = 0;
        virtual void remove(const T& t) = 0;
        virtual void removeAt(size_t idx) = 0;
        virtual void clear() = 0;

    public:
        const T& front() const { return const_cast<AbstractContainer*>(this)->front(); }
        const T& back() const { return const_cast<AbstractContainer*>(this)->back(); }
        const T& at(size_t idx) const { return const_cast<AbstractContainer*>(this)->at(idx); }
        bool contains(const T& t) const { return this->indexOf(t) != RD_NPOS; }

        bool get(size_t idx, T* t) const {
            if(idx >= this->size()) return false;
            if(t) *t = this->at(idx);
            return true;
        }
};
