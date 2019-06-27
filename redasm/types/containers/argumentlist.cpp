#include "argumentlist.h"
#include <impl/support/containers/argumentlist_impl.h>

namespace REDasm {

ArgumentList::ArgumentList(): m_pimpl_p(new ArgumentListImpl()) { }
ArgumentList::ArgumentList(const std::initializer_list<Variant> &args): m_pimpl_p(new ArgumentListImpl()) { }
size_t ArgumentList::size() const { PIMPL_P(const ArgumentList); return p->size(); }
bool ArgumentList::expect(const std::initializer_list<argument_t> &args) const { PIMPL_P(const ArgumentList); return p->expect(args); }
ArgumentList &ArgumentList::operator<<(const Variant &rhs) { PIMPL_P(ArgumentList); p->m_args.append(rhs); return *this;  }
const Variant &ArgumentList::first() const { PIMPL_P(const ArgumentList); return p->m_args.first(); }
const Variant &ArgumentList::last() const { PIMPL_P(const ArgumentList); return p->m_args.last(); }
const Variant &ArgumentList::operator[](size_t idx) const { PIMPL_P(const ArgumentList); return p->m_args[idx]; }

} // namespace REDasm
