#pragma once

#include <cstddef>
#include "../../pimpl.h"

namespace REDasm {

class EventArgs
{
    public:
        typedef size_t Id;

    public:
        EventArgs() = default;
        virtual ~EventArgs() = default;
};

template<typename T>
class ValueEventArgs: public EventArgs
{
    public:
        ValueEventArgs(T val): value(val) { }

    public:
        T value;
};

using BoolEventArgs = ValueEventArgs<bool>;

} // namespace REDasm

