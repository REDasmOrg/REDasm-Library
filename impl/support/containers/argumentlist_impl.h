#pragma once

#include <redasm/types/containers/argumentlist.h>
#include <redasm/types/containers/list.h>

namespace REDasm {

class ArgumentListImpl
{
    PIMPL_DECLARE_Q(ArgumentList)
    PIMPL_DECLARE_PUBLIC(ArgumentList)

    public:
        ArgumentListImpl();
        ArgumentListImpl(const std::initializer_list<Variant> &args);
        size_t size() const;
        bool expect(const std::initializer_list<argument_t> &args) const;

    private:
        List m_args;
};

} // namespace REDasm
