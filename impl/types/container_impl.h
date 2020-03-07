#pragma once

#include <deque>
#include <list>
#include <redasm/types/container.h>

namespace REDasm {

template<typename T> class ListAdapterT: public ListAdapter<T>    // Copy
{
    public:
        typedef std::deque<T> Container;

    public:
        ListAdapterT(const Container& c): m_c(c) { }
        size_t size() const override { return m_c->size(); }
        const T& at(size_t idx) const override { return m_c->operator[](idx); }

    private:
        Container m_c;
};

template<typename T> class ListAdapterRefT: public ListAdapter<T> // Pointer
{
    public:
        typedef std::deque<T> Container;

    public:
        ListAdapterRefT(Container* c): m_c(c) { }
        size_t size() const override { return m_c->size(); }
        const T& at(size_t idx) const override { return m_c->operator[](idx); }

    private:
        Container* m_c;
};

template<typename T> ListAdapter<T>* list_adapter(const typename ListAdapterT<T>::Container& t) { return new ListAdapterT<T>(t); }
template<typename T> ListAdapter<T>* list_adapter(typename ListAdapterRefT<T>::Container* t) { return new ListAdapterRefT<T>(t); }

} // namespace REDasm
