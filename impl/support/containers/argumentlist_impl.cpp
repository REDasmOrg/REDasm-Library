#include "argumentlist_impl.h"

namespace REDasm {

ArgumentListImpl::ArgumentListImpl() { }

ArgumentListImpl::ArgumentListImpl(const std::initializer_list<Variant> &args)
{
    for(auto it = args.begin(); it != args.end(); it++)
        m_args.append(*it);
}

size_t ArgumentListImpl::size() const { return m_args.size(); }

bool ArgumentListImpl::expect(const std::initializer_list<argument_t> &args) const
{
    if(m_args.size() != args.size())
        return false;

    size_t i = 0;

    for(auto it = args.begin(); it != args.end(); it++, i++)
    {
        if((*it == ArgumentList::INTEGER) && m_args[i].isInteger())
            continue;

        if(m_args[i].isObject())
        {
            if(m_args.objectId() != *it)
                return false;

            continue;
        }

        if(static_cast<argument_t>(m_args[i].type()) != *it)
            return false;
    }

    return true;
}

} // namespace REDasm
