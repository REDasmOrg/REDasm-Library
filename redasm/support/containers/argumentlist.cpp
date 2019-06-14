#include "argumentlist.h"
#include <impl/support/containers/argumentlist_impl.h>

namespace REDasm {

Argument::Argument(int8_t v): type(ArgumentType::Integer), i_value(v) { }
Argument::Argument(uint8_t v): type(ArgumentType::Integer), i_value(v) { }
Argument::Argument(int16_t v): type(ArgumentType::Integer), i_value(v) { }
Argument::Argument(uint16_t v): type(ArgumentType::Integer), i_value(v) { }
Argument::Argument(int32_t v): type(ArgumentType::Integer), i_value(v) { }
Argument::Argument(uint32_t v): type(ArgumentType::Integer), i_value(v) { }
Argument::Argument(int64_t v): type(ArgumentType::Integer), i_value(v) { }
Argument::Argument(uint64_t v): type(ArgumentType::Integer), i_value(v) { }
Argument::Argument(float v): type(ArgumentType::Float), d_value(v) { }
Argument::Argument(double v): type(ArgumentType::Float), d_value(v) { }
Argument::Argument(const char *v): type(ArgumentType::String), s_value(v) { }
Argument::Argument(void *v): type(ArgumentType::Pointer), p_value(v) { }

ArgumentList::ArgumentList(): m_pimpl_p(new ArgumentListImpl()) { }
ArgumentList::ArgumentList(const std::initializer_list<Argument> &args): m_pimpl_p(new ArgumentListImpl()) { }
size_t ArgumentList::size() const { PIMPL_P(const ArgumentList); return p->size(); }
bool ArgumentList::expect(const std::initializer_list<ArgumentType> &args) const { PIMPL_P(const ArgumentList); return p->expect(args); }
ArgumentList &ArgumentList::operator<<(uint64_t rhs) { PIMPL_P(ArgumentList); p->m_args.emplace_back(rhs); return *this; }
ArgumentList &ArgumentList::operator<<(double rhs) { PIMPL_P(ArgumentList); p->m_args.emplace_back(rhs); return *this; }
ArgumentList &ArgumentList::operator<<(const char *rhs) { PIMPL_P(ArgumentList); p->m_args.emplace_back(rhs); return *this; }
ArgumentList &ArgumentList::operator<<(void *rhs) { PIMPL_P(ArgumentList); p->m_args.emplace_back(rhs); return *this; }
const Argument &ArgumentList::first() const { PIMPL_P(const ArgumentList); return p->m_args.front(); }
const Argument &ArgumentList::last() const { PIMPL_P(const ArgumentList); return p->m_args.back(); }
const Argument &ArgumentList::operator[](size_t idx) const { PIMPL_P(const ArgumentList); return p->m_args[idx]; }

} // namespace REDasm
