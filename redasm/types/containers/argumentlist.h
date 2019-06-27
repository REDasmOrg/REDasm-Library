#pragma once

#include "../variant.h"
#include "../../pimpl.h"

namespace REDasm {

class ArgumentListImpl;

typedef u32 argument_t;

class ArgumentList
{
    PIMPL_DECLARE_P(ArgumentList)
    PIMPL_DECLARE_PRIVATE(ArgumentList)

    public:
        ArgumentList();
        ArgumentList(const std::initializer_list<Variant>& args);
        size_t size() const;
        bool expect(const std::initializer_list<argument_t>& args) const;
        ArgumentList& operator<<(const Variant& rhs);
        const Variant& first() const;
        const Variant& last() const;
        const Variant& operator[](size_t idx) const;
};

} // namespace REDasm
