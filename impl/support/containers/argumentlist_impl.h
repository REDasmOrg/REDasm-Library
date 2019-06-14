#pragma once

#include <deque>
#include <redasm/support/containers/argumentlist.h>

namespace REDasm {

class ArgumentListImpl
{
    PIMPL_DECLARE_Q(ArgumentList)
    PIMPL_DECLARE_PUBLIC(ArgumentList)

    public:
        ArgumentListImpl();
        ArgumentListImpl(const std::initializer_list<Argument>& args);
        size_t size() const;
        bool expect(const std::initializer_list<ArgumentType>& args) const;

    private:
        std::deque<Argument> m_args;
};

} // namespace REDasm
