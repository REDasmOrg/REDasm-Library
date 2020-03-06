#pragma once

#include "object.h"

namespace REDasm {

template<typename T> class ListAdapter: public REDasm::Object
{
    public:
        virtual size_t size() const = 0;
        virtual const T& at(size_t idx) const = 0;
};

template<typename T> using list_adapter_ptr = REDasm::object_ptr<ListAdapter<T>>;

} // namespace REDasm
