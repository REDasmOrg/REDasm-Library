#include "argumentlist_impl.h"

namespace REDasm {

ArgumentListImpl::ArgumentListImpl() { }

ArgumentListImpl::ArgumentListImpl(const std::initializer_list<Argument> &args)
{
    for(auto it = args.begin(); it != args.end(); it++)
        m_args.push_back(*it);
}

size_t ArgumentListImpl::size() const { return m_args.size(); }

bool ArgumentListImpl::expect(const std::initializer_list<ArgumentType> &args) const
{
    if(m_args.size() != args.size())
        return false;

    size_t i = 0;

    for(auto it = args.begin(); it != args.end(); it++, i++)
    {
        if(m_args[i].type != *it)
            return false;
    }

    return true;
}

} // namespace REDasm
